#include "ImageProcess.h"
#include <opencv2/opencv.hpp>
#include "Filter.h"

int Layer::ID_dispatcher = 0;

void TraceStack::push(MAT changedMat, unsigned layerID)
{
	Trace t{ changedMat, layerID };
	traces.push_back(t);
	if (traces.size() > MAX_TRACES)
		traces.erase(traces.begin());
}

Trace TraceStack::top()
{
	return traces.back();
}

void ImageProcess::revertChange()
{
	auto t = Traces.top();
	std::for_each(Layers.begin(), Layers.end(),
		[t](Layer &layer) { if (t.traceLayerID == layer.getID()) layer.setMat(t.traceValue); });
	Traces.pop();
	//TODO 实现恢复已删除的图层
}

Layer::Layer(const MAT value) :value(value), ID(++ID_dispatcher)
{
	topLeftPoint = { 0,0 };
	bottomRightPoint = { value.mat->cols,value.mat->rows };
}


auto LayerStorage::findLayerByID(unsigned layerID) -> std::deque<Layer>::iterator
{
	auto i = layers.begin();
	for (; i != layers.end(); ++i)
		if (i->getID() == layerID)
			break;
	return i;
}


void LayerStorage::addLayerAsTop(MAT Mat)
{
	Layer newLayer{ Mat };
	addLayerAsTop(newLayer);
}

void LayerStorage::addLayerAsTop(Layer& layer)
{
	layers.push_front(layer);
	layerLevel++;
}


void LayerStorage::addLayerAsBottom(MAT Mat)
{
	Layer newLayer{ Mat };
	addLayerAsBottom(newLayer);
}

void LayerStorage::addLayerAsBottom(Layer& layer)
{
	layers.push_back(layer);
	layerLevel++;
}

Layer& LayerStorage::operator[](int i)
{
	return layers[i];
}

void LayerStorage::addLayerAfter(MAT Mat, int index)
{
	auto layer = layers[index];
	addLayerAfter(Mat, layer);
}

void LayerStorage::addLayerAfter(MAT Mat, Layer& layer)
{
	Layer newLayer{ Mat };
	auto it = findLayerByID(layer.getID());
	if (it == layers.end())
		throw std::runtime_error("Add layer failed. Layer after not found.");
	layers.insert(it, newLayer);
	layerLevel++;
}


void LayerStorage::deleteLayer(Layer& layer)
{
	deleteLayer(layer.getID());
}

void LayerStorage::deleteLayer(unsigned layerID)
{
	auto it = findLayerByID(layerID);
	if (it == layers.end())
		throw std::runtime_error("Delete layer failed. Layer not found. Layer ID =" + std::to_string(layerID));
	layers.erase(it);
	layerLevel--;
}


void LayerStorage::moveLayerUp(Layer& layer)
{
	moveLayerUpByID(layer.getID());
}

void LayerStorage::moveLayerUpByID(unsigned layerID)
{
	auto it = findLayerByID(layerID);
	if (it == layers.end())
		throw std::runtime_error("Move layer failed. Layer not found. Layer ID=" + std::to_string(layerID));
	if (it == layers.begin())
		return;
	auto itafter = it - 1;
	std::swap(*it, *itafter);
}

void LayerStorage::moveLayerUp(int index)
{
	auto it = layers.begin() + index;
	auto itafter = it - 1;
	std::swap(*it, *itafter);
}


void LayerStorage::moveLayerDown(Layer& layer)
{
	moveLayerDownByID(layer.getID());
}

void LayerStorage::moveLayerDownByID(unsigned layerID)
{
	auto it = findLayerByID(layerID);
	if (it == layers.end())
		throw std::runtime_error("Move layer failed. Layer not found. Layer ID=" + std::to_string(layerID));
	if (it == layers.end() - 1)
		return;
	auto itafter = it + 1;
	std::swap(*it, *itafter);
}

void LayerStorage::moveLayerDown(int index)
{
	moveLayerDown(layers[index]);
}

void LayerStorage::moveLayerTo(int index, int targetIndex)
{
	auto it = layers.begin() + index;
	auto targetIt = layers.begin() + targetIndex;
	layers.insert(targetIt, *it);
	layers.erase(it);
}

void LayerStorage::moveLayerTo(Layer& layer, Layer& targetLayer)
{
	moveLayerToByID(layer.getID(), targetLayer.getID());
}

void ImageProcess::NostalgicHue(ImageProcess& process, Layer &layer)
{
	Mat dst;
	process.Traces.push(layer.getMat(), layer.getID());
	try {
		dst = NostalgicHueFilter(parseMAT(layer.getMat()));
	}
	catch (const process_error& e) {
		if (e.get_error_code() == FilterChannelsError) {
			throw FilterChannelsError;
		}
	}
	auto DST = packMAT(dst);
	layer.setMat(DST);
}

void ImageProcess::Sculpture(ImageProcess& process, Layer &layer)
{
	Mat dst;
	process.Traces.push(layer.getMat(), layer.getID());
	try {
		auto mat = parseMAT(layer.getMat());
		dst = SculptureFilter(mat);
	}
	catch (const process_error& e) {
		if (e.get_error_code() == FilterChannelsError) {
			throw FilterChannelsError;
		}
	}
}
void LayerStorage::moveLayerToByID(unsigned layerID, unsigned targetLayerID)
{
	auto it = findLayerByID(layerID);
	auto targetIt = findLayerByID(targetLayerID);
	if (it == layers.end() || targetIt == layers.end())
		throw std::runtime_error("Move layer failed. Layer not found.");
	layers.insert(targetIt, *it);
	layers.erase(it);
}



void LayerStorage::setLayerSizeAsImageSize(int index)
{
	auto topLeftPoint = layers[index].getTopLeftPoint();
	layers[index].setBottomRightPoint(
		topLeftPoint.first + layers[index].getMat().mat->cols,
		topLeftPoint.second + layers[index].getMat().mat->rows);
}

void LayerStorage::setLayerSizeAsImageSize(Layer& layer)
{
	setLayerSizeAsImageSizeByID(layer.getID());
}

void LayerStorage::setLayerSizeAsImageSizeByID(unsigned layerID)
{
	auto it = findLayerByID(layerID);
	if (it == layers.end())
		throw std::runtime_error("Layer not found. Layer ID=" + std::to_string(layerID));
	auto topLeftPoint = it->getTopLeftPoint();
	it->setBottomRightPoint(
		topLeftPoint.first + it->getMat().mat->cols,
		topLeftPoint.second + it->getMat().mat->rows);
}

void LayerStorage::mergeLayers(int frontIndex, int backIndex, double blendAlpha)
{
	auto frontLayer = layers[frontIndex], backLayer = layers[backIndex];
	mergeLayers(frontLayer, backLayer, blendAlpha);
}

bool isPointInImage(std::pair<int, int> point, Layer &layer)
{
	auto topLeftPoint = layer.getTopLeftPoint();
	auto bottomRightPoint = layer.getBottomRightPoint();
	if (point.first > topLeftPoint.first&&point.first<bottomRightPoint.second
		&&point.second>topLeftPoint.second&&point.second < bottomRightPoint.second)
		return true;
	return false;
}

auto getOverlapRelation(Layer &layer1, Layer &layer2)->std::pair<std::pair<int, int>, std::pair<int, int>>
{
	//该函数检查layer1和layer2的关系.
	//根据layer1的四点是否在layer2内,计算layer1和layer2的重叠范围
	std::pair<int, int> overlapTopLeftPoint, overlapBottomRightPoint;

	enum
	{
		NONE_OVERLAP,
		TOP_LEFT,
		TOP_LEFT_RIGHT,
		TOP_RIGHT,
		LEFT_TOP_BOTTOM,
		RIGHT_TOP_BOTTOM,
		BOTTOM_LEFT,
		BOTTOM_LEFT_RIGHT,
		BOTTOM_RIGHT,
		INSIDE
	};

	bool rTopLeft = isPointInImage(layer1.getTopLeftPoint(), layer2),
		rTopRight = isPointInImage(layer1.getTopRightPoint(), layer2),
		rBottomLeft = isPointInImage(layer1.getBottomLeftPoint(), layer2),
		rBottomRight = isPointInImage(layer1.getBottomRightPoint(), layer2);
	int result;
	if (rTopLeft&&rTopRight&&rBottomLeft&&rBottomRight)
		result = INSIDE;
	else if (rTopLeft&&rTopRight)
		result = TOP_LEFT_RIGHT;
	else if (rTopLeft&&rBottomLeft)
		result = LEFT_TOP_BOTTOM;
	else if (rBottomLeft&&rBottomRight)
		result = BOTTOM_LEFT_RIGHT;
	else if (rTopRight&&rBottomRight)
		result = RIGHT_TOP_BOTTOM;
	else if (rTopLeft)
		result = TOP_LEFT;
	else if (rTopRight)
		result = TOP_RIGHT;
	else if (rBottomLeft)
		result = BOTTOM_LEFT;
	else if (rBottomRight)
		result = BOTTOM_RIGHT;
	else
		result = NONE_OVERLAP;

	switch (result)
	{
	case INSIDE:
		overlapTopLeftPoint = layer1.getTopLeftPoint();
		overlapBottomRightPoint = layer1.getBottomRightPoint();
		break;
	case TOP_LEFT_RIGHT:
		overlapTopLeftPoint = layer1.getTopLeftPoint();
		overlapBottomRightPoint.first = layer1.getBottomRightPoint().first;
		overlapBottomRightPoint.second = layer2.getBottomRightPoint().second;
		break;
	case LEFT_TOP_BOTTOM:
		overlapTopLeftPoint = layer1.getTopLeftPoint();
		overlapBottomRightPoint.first = layer2.getBottomRightPoint().first;
		overlapBottomRightPoint.second = layer1.getBottomRightPoint().second;
		break;
	case BOTTOM_LEFT_RIGHT:
		overlapTopLeftPoint.first = layer1.getTopLeftPoint().first;
		overlapTopLeftPoint.second = layer2.getTopLeftPoint().second;
		overlapBottomRightPoint = layer1.getBottomRightPoint();
		break;
	case RIGHT_TOP_BOTTOM:
		overlapTopLeftPoint.first = layer2.getTopLeftPoint().first;
		overlapTopLeftPoint.second = layer1.getTopLeftPoint().second;
		overlapBottomRightPoint = layer1.getBottomRightPoint();
		break;
	case TOP_LEFT:
		overlapTopLeftPoint = layer1.getTopLeftPoint();
		overlapBottomRightPoint = layer2.getBottomRightPoint();
		break;
	case TOP_RIGHT:
		overlapTopLeftPoint.first = layer2.getTopLeftPoint().first;
		overlapTopLeftPoint.second = layer1.getTopLeftPoint().second;
		overlapBottomRightPoint.first = layer1.getBottomRightPoint().first;
		overlapBottomRightPoint.second = layer2.getBottomRightPoint().second;
		break;
	case BOTTOM_LEFT:
		overlapTopLeftPoint.first = layer1.getTopLeftPoint().first;
		overlapTopLeftPoint.second = layer2.getTopLeftPoint().second;
		overlapBottomRightPoint.first = layer2.getBottomRightPoint().first;
		overlapBottomRightPoint.second = layer1.getBottomRightPoint().second;
		break;
	case BOTTOM_RIGHT:
		overlapTopLeftPoint = layer2.getTopLeftPoint();
		overlapBottomRightPoint = layer1.getBottomRightPoint();
		break;
	case NONE_OVERLAP:
	default:
		overlapTopLeftPoint = { 0,0 };
		overlapBottomRightPoint = { 0,0 };
	}


	return { overlapTopLeftPoint,overlapBottomRightPoint };
}

auto getOverlapArea(Layer &layer1, Layer &layer2)
{
	auto r = getOverlapRelation(layer1, layer2);
	if (r.first != std::pair<int, int>(0, 0)
		|| r.second != std::pair<int, int>(0, 0))
		return r;
	//如果1->2的关系为None,并不意味着两个图层一定不相交
	//有可能是1的顶点不在2内,但是2的顶点在1内
	//因此需要检查2->1的关系
	r = getOverlapRelation(layer2, layer1);

	return r;
}

void LayerStorage::mergeLayers(Layer& frontLayer, Layer& backLayer, double blendAlpha)
{
	auto frontMat = *frontLayer.getMat().mat;
	auto backMat = *backLayer.getMat().mat;
	//求两个图像的重叠范围
	auto overlap = getOverlapArea(frontLayer, backLayer);

	//构造可以容下两个图像的新图像
	std::pair<int, int> newTopLeft, newBottomRight;
	newTopLeft.first = std::min(frontLayer.getTopLeftPoint().first, backLayer.getTopLeftPoint().first);
	newTopLeft.second = std::min(frontLayer.getTopLeftPoint().second, backLayer.getTopLeftPoint().second);
	newBottomRight.first = std::max(frontLayer.getBottomRightPoint().first, backLayer.getBottomRightPoint().first);
	newBottomRight.second = std::max(frontLayer.getBottomRightPoint().second, backLayer.getBottomRightPoint().second);

	int newWidth = newBottomRight.first - newTopLeft.first,
		newHeight = newBottomRight.second - newTopLeft.second;
	//todo 此处的type直接使用了frontmat的type,实际上应该使用可兼容frontmat和backmat的type
	cv::Mat newMat(newHeight, newWidth, frontMat.type());
	Layer newLayer(packMAT(newMat), newTopLeft.first, newTopLeft.second, newBottomRight.first, newBottomRight.second);

	//构造新图像中两个旧图像的ROI
	cv::Rect newFrontArea(frontLayer.getTopLeftPoint().first - newTopLeft.first,
		frontLayer.getTopLeftPoint().second - newTopLeft.second,
		frontLayer.getBottomRightPoint().first - frontLayer.getTopLeftPoint().first,
		frontLayer.getBottomRightPoint().second - frontLayer.getTopLeftPoint().second);
	cv::Rect newBackArea(backLayer.getTopLeftPoint().first - newTopLeft.first,
		backLayer.getTopLeftPoint().second - newTopLeft.second,
		backLayer.getBottomRightPoint().first - backLayer.getTopLeftPoint().first,
		backLayer.getBottomRightPoint().second - backLayer.getTopLeftPoint().second);

	auto frontROI = newMat(newFrontArea);
	auto backROI = newMat(newBackArea);

	//如果两个图像有重叠部分,则构造两个旧图像中重叠位置的ROI,并完成混合
	if (overlap.first != overlap.second)
	{
		//构造ROI
		cv::Rect frontOverlapArea(overlap.first.first - frontLayer.getTopLeftPoint().first,
			overlap.first.second - frontLayer.getTopLeftPoint().second,
			overlap.second.first - overlap.first.first,
			overlap.second.second - overlap.first.second
		);
		cv::Rect backOverlapArea(overlap.first.first - backLayer.getTopLeftPoint().first,
			overlap.first.second - backLayer.getTopLeftPoint().second,
			overlap.second.first - overlap.first.first,
			overlap.second.second - overlap.first.second
		);
		auto frontBlendROI = frontMat(frontOverlapArea);
		auto backBlendROI = backMat(backOverlapArea);

		//将重叠位置的混合结果存放在frongLayer中
		cv::addWeighted(frontBlendROI, blendAlpha,
			backBlendROI, 1 - blendAlpha,
			0.0, frontBlendROI);
	}

	//将结果复制到新图层上
	cv::addWeighted(backROI, 0.0, backMat, 1.0, 0.0, backROI);
	cv::addWeighted(frontROI, 0.0, frontMat, 1.0, 0.0, frontROI);

	//清理图层
	deleteLayer(frontLayer);
	deleteLayer(backLayer);

	addLayerAsTop(newLayer);

}

void LayerStorage::mergeLayersByID(unsigned frontLayerID, unsigned backLayerID, double blendAlpha)
{
	auto frontLayerIt = findLayerByID(frontLayerID);
	auto backLayerIt = findLayerByID(backLayerID);
	mergeLayers(*frontLayerIt, *backLayerIt, blendAlpha);
}

void ImageProcess::AdjustContrastAndBrightness(ImageProcess& process, Layer& layer, double contrast, double brightness)
{
	auto mat = parseMAT(layer.getMat());
	process.Traces.push(layer.getMat(), layer.getID());
	cv::Mat dst;
	mat.convertTo(dst, -1, contrast, brightness);
	MAT DST = packMAT(dst);
	layer.setMat(DST);
}

void ImageProcess::GammaCorrection(ImageProcess& process, Layer& layer, double gamma)
{
	cv::Mat lookUpTable(1, 256, CV_8U);
	auto p = lookUpTable.ptr();
	for (int i = 0; i < 256; ++i)
		p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma) * 255);

	auto mat = parseMAT(layer.getMat());
	process.Traces.push(layer.getMat(), layer.getID());
	cv::Mat dst = mat.clone();
	cv::LUT(mat, lookUpTable, dst);

	MAT DST = packMAT(dst);
	layer.setMat(DST);
}


void ImageProcess::GaussianBlur(ImageProcess &process, Layer &layer, double strength)
{
	auto mat = parseMAT(layer.getMat());

	//修改前存储该图层的原图像,作为撤销操作的备份
	process.Traces.push(layer.getMat(), layer.getID());

	cv::Mat dst;
	//对strength做线性插值
	int size = 3 + strength * (std::min(mat.rows, mat.cols) / 10.0 - 3);
	//kernel必须是奇数尺寸
	if (size % 2 == 0) ++size;
	cv::Size kernel_size(size, size);

	//自己编写的处理函数的参数列表要类似cv库中函数的写法(如下GaussianBlur)
	cv::GaussianBlur(mat, dst, kernel_size, 0);

	MAT DST = packMAT(dst);
	layer.setMat(DST);
}

void ImageProcess::StrongLight(ImageProcess& process, Layer &layer)
{
	Mat dst;
	process.Traces.push(layer.getMat(), layer.getID());
	try {
		auto mat = parseMAT(layer.getMat());
		dst = StrongLightFilter(mat);
	}
	catch (const process_error& e) {
		if (e.get_error_code() == FilterChannelsError) {
			throw FilterChannelsError;
		}
	}
	auto DST = packMAT(dst);
	layer.setMat(DST);
}

void ImageProcess::Feather(ImageProcess& process, Layer &layer, double VagueRatio)
{
	Mat dst;
	process.Traces.push(layer.getMat(), layer.getID());
	try {
		auto mat = parseMAT(layer.getMat());
		dst = FeatherFilter(mat, VagueRatio);
	}
	catch (const process_error& e) {
		if (e.get_error_code() == FilterChannelsError) {
			throw FilterChannelsError;
		}
	}
	auto DST = packMAT(dst);
	layer.setMat(DST);
}

void ImageProcess::DarkTown(ImageProcess& process, Layer &layer, double DarkDegree)
{
	Mat dst;
	process.Traces.push(layer.getMat(), layer.getID());
	try {
		auto mat = parseMAT(layer.getMat());
		dst = DarkTownFilter(mat, DarkDegree);
	}
	catch (const process_error& e) {
		if (e.get_error_code() == FilterChannelsError) {
			throw FilterChannelsError;
		}
	}
	auto DST = packMAT(dst);
	layer.setMat(DST);
}

void ImageProcess::Mosaic(ImageProcess& process, Layer &layer, int size)
{
	Mat dst;
	process.Traces.push(layer.getMat(), layer.getID());
	try {
		auto mat = parseMAT(layer.getMat());
		dst = MosaicFilter(mat, size);
	}
	catch (const process_error& e) {
		if (e.get_error_code() == FilterChannelsError) {
			throw FilterChannelsError;
		}
	}
	auto DST = packMAT(dst);
	layer.setMat(DST);
}

void ImageProcess::Diffusion(ImageProcess& process, Layer &layer)
{
	Mat dst;
	process.Traces.push(layer.getMat(), layer.getID());
	try {
		auto mat = parseMAT(layer.getMat());
		dst = DiffusionFilter(mat);
	}
	catch (const process_error& e) {
		if (e.get_error_code() == FilterChannelsError) {
			throw FilterChannelsError;
		}
	}
	auto DST = packMAT(dst);
	layer.setMat(DST);
}

void ImageProcess::Wind(ImageProcess& process, Layer &layer, int strength)
{
	Mat dst;
	process.Traces.push(layer.getMat(), layer.getID());
	try {
		auto mat = parseMAT(layer.getMat());
		dst = WindFilter(mat, strength);
	}
	catch (const process_error& e) {
		if (e.get_error_code() == FilterChannelsError) {
			throw FilterChannelsError;
		}
	}
	auto DST = packMAT(dst);
	layer.setMat(DST);
}