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
	return traces.front();
}


cv::Mat& ImageProcess::parseMAT(MAT Mat)
{
	return *Mat.mat;
}

MAT ImageProcess::packMAT(cv::Mat mat)
{
	return { std::make_shared<cv::Mat>(mat) };
}

void ImageProcess::revertChange()
{
	auto t = Traces.top();
	std::for_each(Layers.begin(), Layers.end(),
		[t](Layer &layer) { if (t.traceLayerID == layer.getID()) layer.setMat(t.traceValue); });
	//TODO 实现恢复已删除的图层
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
	auto it = std::find_if(layers.begin(), layers.end(),
		[layer](Layer &L) {return L.getID() == layer.getID(); });
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
	auto it = std::find_if(layers.begin(), layers.end(),
		[layerID](Layer &L) {return L.getID() == layerID; });
	if (it == layers.end())
		throw std::runtime_error("Delete layer failed. Layer not found. Layer ID =" + std::to_string(layerID));
	//Traces.push(it->getMat(), it->getID());
	layers.erase(it);
	layerLevel--;
}


void LayerStorage::moveLayerUp(Layer& layer)
{
	moveLayerUpByID(layer.getID());
}

void LayerStorage::moveLayerUpByID(unsigned layerID)
{
	auto it = std::find_if(layers.begin(), layers.end(),
		[layerID](Layer &L) {return L.getID() == layerID; });
	if (it == layers.end())
		throw std::runtime_error("Move layer failed. Layer not found. Layer ID=" + std::to_string(layerID));
	if (it == layers.begin())
		return;
	auto itafter = it - 1;
	std::swap(*it, *itafter);
}

void LayerStorage::moveLayerUp(int index)
{
	moveLayerUp(layers[index]);
}


void LayerStorage::moveLayerDown(Layer& layer)
{
	moveLayerDownByID(layer.getID());
}

void LayerStorage::moveLayerDownByID(unsigned layerID)
{
	auto it = std::find_if(layers.begin(), layers.end(),
		[layerID](Layer &L) {return L.getID() == layerID; });
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

void ImageProcess::Sculpture(ImageProcess& process, Layer &layer)
{
	process.Traces.push(layer.getMat(), layer.getID());
	auto dst = SculptureFilter(parseMAT(layer.getMat()));
	auto DST = packMAT(dst);
	layer.setMat(DST);
}
