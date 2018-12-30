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
	//TODO ʵ�ָֻ���ɾ����ͼ��
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
	//�ú������layer1��layer2�Ĺ�ϵ.
	//����layer1���ĵ��Ƿ���layer2��,����layer1��layer2���ص���Χ
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
	//���1->2�Ĺ�ϵΪNone,������ζ������ͼ��һ�����ཻ
	//�п�����1�Ķ��㲻��2��,����2�Ķ�����1��
	//�����Ҫ���2->1�Ĺ�ϵ
	r = getOverlapRelation(layer2, layer1);

	return r;
}

void LayerStorage::mergeLayers(Layer& frontLayer, Layer& backLayer, double blendAlpha)
{
	auto frontMat = *frontLayer.getMat().mat;
	auto backMat = *backLayer.getMat().mat;
	//������ͼ����ص���Χ
	auto overlap = getOverlapArea(frontLayer, backLayer);

	//���������������ͼ�����ͼ��
	std::pair<int, int> newTopLeft, newBottomRight;
	newTopLeft.first = std::min(frontLayer.getTopLeftPoint().first, backLayer.getTopLeftPoint().first);
	newTopLeft.second = std::min(frontLayer.getTopLeftPoint().second, backLayer.getTopLeftPoint().second);
	newBottomRight.first = std::max(frontLayer.getBottomRightPoint().first, backLayer.getBottomRightPoint().first);
	newBottomRight.second = std::max(frontLayer.getBottomRightPoint().second, backLayer.getBottomRightPoint().second);

	int newWidth = newBottomRight.first - newTopLeft.first,
		newHeight = newBottomRight.second - newTopLeft.second;
	//todo �˴���typeֱ��ʹ����frontmat��type,ʵ����Ӧ��ʹ�ÿɼ���frontmat��backmat��type
	cv::Mat newMat(newHeight, newWidth, frontMat.type());
	Layer newLayer(packMAT(newMat), newTopLeft.first, newTopLeft.second, newBottomRight.first, newBottomRight.second);

	//������ͼ����������ͼ���ROI
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

	//�������ͼ�����ص�����,����������ͼ�����ص�λ�õ�ROI,����ɻ��
	if (overlap.first != overlap.second)
	{
		//����ROI
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

		//���ص�λ�õĻ�Ͻ�������frongLayer��
		cv::addWeighted(frontBlendROI, blendAlpha,
			backBlendROI, 1 - blendAlpha,
			0.0, frontBlendROI);
	}

	//��������Ƶ���ͼ����
	cv::addWeighted(backROI, 0.0, backMat, 1.0, 0.0, backROI);
	cv::addWeighted(frontROI, 0.0, frontMat, 1.0, 0.0, frontROI);

	//����ͼ��
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

	//�޸�ǰ�洢��ͼ���ԭͼ��,��Ϊ���������ı���
	process.Traces.push(layer.getMat(), layer.getID());

	cv::Mat dst;
	//��strength�����Բ�ֵ
	int size = 3 + strength * (std::min(mat.rows, mat.cols) / 10.0 - 3);
	//kernel�����������ߴ�
	if (size % 2 == 0) ++size;
	cv::Size kernel_size(size, size);

	//�Լ���д�Ĵ������Ĳ����б�Ҫ����cv���к�����д��(����GaussianBlur)
	cv::GaussianBlur(mat, dst, kernel_size, 0);

	MAT DST = packMAT(dst);
	layer.setMat(DST);
}

void ImageProcess::Sculpture(ImageProcess& process, Layer &layer)
{
	process.Traces.push(layer.getMat(), layer.getID());
	auto dst = SculptureFilter(parseMAT(layer.getMat()));
	auto DST = packMAT(dst);
	layer.setMat(DST);
}