#include "ImageProcess.h"
#include <opencv2/opencv.hpp>
#include "Filter.h"

int Layer::ID_dispatcher = 0;
double penParameter::B = 0.0;
double penParameter::G = 0.0;
double penParameter::R = 0.0;
int penParameter::size = 2;
double penParameter::scale = 1;
FondFace penParameter::face = CV_FONT_HERSHEY_SIMPLEX;

void TraceStack::push(MAT changedMat, unsigned layerID)
{
	Trace t{ changedMat, layerID };
	traces.push_back(t);
	if (traces.size() > MAX_TRACES)
		traces.erase(traces.begin());
}

Trace TraceStack::top()
{
	return traces.front();
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

Layer::Layer(ElementType e, int leftUpX, int leftUpY, int rightDownX, int rightDownY)
	:attachment(e, penParameter::getPenSize(), penParameter::getPenColorB(), penParameter::getPenColorG(), penParameter::getPenColorR())
{
	ID = ++ID_dispatcher;
	topLeftPoint = { leftUpX, leftUpY };
	bottomRightPoint = { rightDownX, rightDownY };
	property = DRAW | PRIMITIVE;
}

Layer::Layer(std::string s, int x, int y)
	:attachment(s, penParameter::getPenColorB(), penParameter::getPenColorG(), penParameter::getPenColorR()
		, penParameter::getPenFace(), penParameter::getPenScale(), penParameter::getPenSize())
{
	ID = ++ID_dispatcher;
	cv::Size text_size = cv::getTextSize(s, penParameter::getPenFace(), penParameter::getPenScale(),
		(penParameter::getPenSize() >= 1 ? penParameter::getPenSize() : 1), 0);
	topLeftPoint = { x, y - text_size.height };
	bottomRightPoint = { x + text_size.width, y };
	property = DRAW | TEXT;
}

MAT Layer::getMat() {
	if (checkProperty(RASTERIZED)) {
		;
	}
	else if (checkProperty(PRIMITIVE)) {
		ImageProcess::drawPrimitive(this->value, getLayerAttachment().gettype(), getTopLeftPoint().first, getTopLeftPoint().second,
			getBottomRightPoint().first, getBottomRightPoint().second, getLayerAttachment().getPenSize(),
			getLayerAttachment().getPenColorB(), getLayerAttachment().getPenColorG(), getLayerAttachment().getPenColorR());
	}
	else if (checkProperty(TEXT)) {
		ImageProcess::drawText(this->value, getLayerAttachment().getText(), getTopLeftPoint().first, getTopLeftPoint().second,
			getBottomRightPoint().first, getBottomRightPoint().second,
			getLayerAttachment().getThickness(), getLayerAttachment().getScale(), getLayerAttachment().getFace(),
			getLayerAttachment().getPenColorB(), getLayerAttachment().getPenColorG(), getLayerAttachment().getPenColorR());
	}
	return value;
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


//图元图层函数
void ImageProcess::drawPrimitive(MAT& value, ElementType e, int leftUpX, int leftUpY, int rightDownX, int rightDownY,int size, double B, double G, double R) {

	Mat src(rightDownY - leftUpY + size, rightDownX - leftUpX + size , CV_8UC4, cv::Scalar(255, 255, 255, 0));
	//cv并未提供现成的三角形绘制函数，采用点集画图
	std::vector<cv::Point > contour;
	std::vector<std::vector<cv::Point >> contours;


	switch (e)
	{
	case ELLIPSE:
		ellipse(src, cv::Point((rightDownX - leftUpX) / 2, (rightDownY - leftUpY) / 2), cv::Size((rightDownX - leftUpX) / 2, (rightDownY - leftUpY) / 2),
			0, 0, 360, { B,G,R ,255 }, size, cv::LINE_AA);
		break;
	case LINE1:
		line(src, cv::Point(0, 0), cv::Point(rightDownX - leftUpX, rightDownY - leftUpY), { B,G,R ,255 }, (size >= 1 ? size : 1), cv::LINE_AA);			//受CV绘图函数限制，只提供矩形和椭圆的填充效果
		break;
	case LINE2:
		line(src, cv::Point(0, rightDownY - leftUpY), cv::Point(rightDownX - leftUpX, 0), { B,G,R ,255 }, (size >= 1 ? size : 1), cv::LINE_AA);			//受CV绘图函数限制，只提供矩形和椭圆的填充效果
		break;
	case TRIANGLE:
		contour.reserve(3);
		contour.push_back(cv::Point(0, rightDownY - leftUpY));
		contour.push_back(cv::Point(rightDownX - leftUpX, rightDownY - leftUpY));
		contour.push_back(cv::Point((rightDownX - leftUpX) / 2, 0));
		contours.push_back(contour);
		cv::polylines(src, contours, true, cv::Scalar(B, G, R, 255), (size >= 1 ? size : 1), cv::LINE_AA);
		if (size == -1)cv::fillPoly(src, contours, cv::Scalar(B, G, R, 255));
		//line(src, cv::Point(leftUpX, rightDownY), cv::Point(rightDownX, rightDownY), { B,G,R ,255 }, (size >= 1 ? size : 1), cv::LINE_AA);
		//line(src, cv::, cv::Point((leftUpX + rightDownX) / 2, leftUpY), { B,G,R ,255 }, (size >= 1 ? size : 1), cv::LINE_AA);
		//line(src, cv::Point((leftUpX + rightDownX) / 2, leftUpY), cv::Point(leftUpX, rightDownY), { B,G,R ,255 }, (size >= 1 ? size : 1), cv::LINE_AA);
		break;
	case RECTANGULAR:
		rectangle(src, cv::Point(0, 0), cv::Point(rightDownX, rightDownY), { B,G,R ,255 }, size, cv::LINE_AA);
		break;
	default:
		break;
	}
	value = packMAT(src);
}

void ImageProcess::movePrimitive(ImageProcess &process, Layer &layer, int dx, int dy) {

	int leftUpX = layer.getTopLeftPoint().first + dx;
	int leftUpY = layer.getTopLeftPoint().second + dy;
	int rightDownX = layer.getBottomRightPoint().first + dx;
	int rightDownY = layer.getBottomRightPoint().second + dy;
	layer.setTopLeftPoint(leftUpX, leftUpY);
	layer.setBottomRightPoint(rightDownX, rightDownY);

}

void ImageProcess::scalePrimitive(ImageProcess &process, Layer &layer, int leftUpdx, int leftUpdy, int RightDowndx, int RightDowndy) {

	int leftUpX = layer.getTopLeftPoint().first + leftUpdx;
	int leftUpY = layer.getTopLeftPoint().second + leftUpdy;
	int rightDownX = layer.getBottomRightPoint().first + RightDowndx;
	int rightDownY = layer.getBottomRightPoint().second + RightDowndy;
	/*
	switch (p)
	{
	case TOP_LEFT:
	leftUpX += dx;
	leftUpY += dy;
	break;
	case TOP:
	leftUpY += dy;
	break;
	case TOP_RIGHT:
	leftUpY += dy;
	rightDownX += dx;
	break;
	case RIGHT:
	rightDownX += dx;
	break;
	case BOTTOM_RIGHT:
	rightDownX += dx;
	rightDownY += dy;
	break;
	case BOTTON:
	rightDownY += dy;
	break;
	case BOTTOM_LEFT:
	leftUpX += dx;
	rightDownY += dy;
	break;
	case LEFT:
	leftUpX += dx;
	break;
	default:
	break;
	}

	*/
	
	layer.setTopLeftPoint(leftUpX, leftUpY);
	layer.setBottomRightPoint(rightDownX, rightDownY);

}

void ImageProcess::changePrimitiveColor(ImageProcess &process, Layer &layer, double B, double G, double R) {

	layer.getLayerAttachment().setPenColor(B, G, R);
	penParameter::setPenColor(B, G, R);

}

void ImageProcess::changePrimitivePenSize(ImageProcess &process, Layer &layer, int size) {
	
	//size=-
	penParameter::setPenSize(size);
	layer.getLayerAttachment().setPenSize(size);

}

//文字图层函数
void ImageProcess::drawText(MAT& src, std::string s, int leftUpX, int leftUpY, int rightDownX, int rightDownY,
	int size, int scale, FondFace face, double B, double G, double R) {

	Mat NewSrc(rightDownY - leftUpY + 2, rightDownX - leftUpX + 2, CV_8UC4, cv::Scalar(255, 255, 255, 0));
	cv::putText(NewSrc, s, { 0, rightDownY - leftUpY }, face, scale, cv::Scalar(B, G, R, 255), (size >= 1 ? size : 1), cv::LINE_AA, 0);

	src = packMAT(NewSrc);
}


void ImageProcess::changeTextColor(ImageProcess &process, Layer &layer, double B, double G, double R) {
	layer.getLayerAttachment().setPenColor(B, G, R);
	penParameter::setPenColor(B, G, R);
}

void ImageProcess::changeTextThickness(ImageProcess &process, Layer &layer, int thickness){

	layer.getLayerAttachment().setThickness(thickness);
	//修改粗细后重新确定Layer绘制的位置，后面的方法皆如此。
	cv::Size text_size = cv::getTextSize(layer.getLayerAttachment().getText(), layer.getLayerAttachment().getFace(),
		layer.getLayerAttachment().getScale(),(layer.getLayerAttachment().getThickness() >= 1 ? layer.getLayerAttachment().getThickness() : 1), 0);
	int topLeftX = layer.getTopLeftPoint().first;
	int	topLeftY = layer.getBottomRightPoint().second - text_size.height;
	int bottomRightX= layer.getTopLeftPoint().first + text_size.width;
	int bottomRightY = layer.getBottomRightPoint().second;
	layer.setTopLeftPoint(topLeftX, topLeftY);
	layer.setBottomRightPoint(bottomRightX, bottomRightY);
	penParameter::setPenSize(thickness);
}

void ImageProcess::changeTextScale(ImageProcess &process, Layer &layer, int scale) {
	
	layer.getLayerAttachment().setScale(scale);
	cv::Size text_size = cv::getTextSize(layer.getLayerAttachment().getText(), layer.getLayerAttachment().getFace(),
		layer.getLayerAttachment().getScale(), (layer.getLayerAttachment().getThickness() >= 1 ? layer.getLayerAttachment().getThickness() : 1), 0);
	int topLeftX = layer.getTopLeftPoint().first;
	int	topLeftY = layer.getBottomRightPoint().second - text_size.height;
	int bottomRightX = layer.getTopLeftPoint().first + text_size.width;
	int bottomRightY = layer.getBottomRightPoint().second;
	layer.setTopLeftPoint(topLeftX, topLeftY);
	layer.setBottomRightPoint(bottomRightX, bottomRightY);
	penParameter::setPenScale(scale);
}

void ImageProcess::changeTextFace(ImageProcess &process, Layer &layer, FondFace face) {
	layer.getLayerAttachment().setFace(face);
	cv::Size text_size = cv::getTextSize(layer.getLayerAttachment().getText(), layer.getLayerAttachment().getFace(),
		layer.getLayerAttachment().getScale(), (layer.getLayerAttachment().getThickness() >= 1 ? layer.getLayerAttachment().getThickness() : 1), 0);
	int topLeftX = layer.getTopLeftPoint().first;
	int	topLeftY = layer.getBottomRightPoint().second - text_size.height;
	int bottomRightX = layer.getTopLeftPoint().first + text_size.width;
	int bottomRightY = layer.getBottomRightPoint().second;
	layer.setTopLeftPoint(topLeftX, topLeftY);
	layer.setBottomRightPoint(bottomRightX, bottomRightY);
	penParameter::setPenFace(face);
}

void ImageProcess::moveText(ImageProcess &process, Layer &layer, int dx, int dy) {

	int leftUpX = layer.getTopLeftPoint().first + dx;
	int leftUpY = layer.getTopLeftPoint().second + dy;
	int rightDownX = layer.getBottomRightPoint().first + dx;
	int rightDownY = layer.getBottomRightPoint().second + dy;
	layer.setTopLeftPoint(leftUpX, leftUpY);
	layer.setBottomRightPoint(rightDownX, rightDownY);

}

void ImageProcess::rewriteText(ImageProcess &process, Layer &layer, std::string t) {

	layer.getLayerAttachment().setText(t);
}


//滤镜函数
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

void ImageProcess::NostalgicHue(ImageProcess& process, Layer &layer)
{
	Mat dst;
	process.Traces.push(layer.getMat(), layer.getID());
	try {
		dst = NostalgicHueFilter(parseMAT(layer.getMat()));
	}
	catch (const process_error& e) {
		if (e.get_error_code() == FILTER_CHANNELS_ERROR) {
			throw FILTER_CHANNELS_ERROR;
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
		if (e.get_error_code() == FILTER_CHANNELS_ERROR) {
			throw FILTER_CHANNELS_ERROR;
		}
	}
	auto DST = packMAT(dst);
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
		if (e.get_error_code() == FILTER_CHANNELS_ERROR) {
			throw FILTER_CHANNELS_ERROR;
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
		if (e.get_error_code() == FILTER_CHANNELS_ERROR) {
			throw FILTER_CHANNELS_ERROR;
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
		if (e.get_error_code() == FILTER_CHANNELS_ERROR) {
			throw FILTER_CHANNELS_ERROR;
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
		if (e.get_error_code() == FILTER_CHANNELS_ERROR) {
			throw FILTER_CHANNELS_ERROR;
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
		if (e.get_error_code() == FILTER_CHANNELS_ERROR) {
			throw FILTER_CHANNELS_ERROR;
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
		if (e.get_error_code() == FILTER_CHANNELS_ERROR) {
			throw FILTER_CHANNELS_ERROR;
		}
	}
	auto DST = packMAT(dst);
	layer.setMat(DST);
}
