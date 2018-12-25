#ifndef _IMAGE_PROCESS_H
#define _IMAGE_PROCESS_H
#include <deque>
#include <vector>
#include "MAT.h"



class Layer
{
private:
	MAT value;
	unsigned ID;
	//利用左上点和右下点标记图层的大小.图层大小只影响绘制时的大小,不影响该图层的value的矩阵尺寸
	std::pair<int, int> topLeftPoint, bottomRightPoint;

	static int ID_dispatcher;
public:
	Layer(const MAT value);
	Layer(const MAT value, int leftUpX, int leftUpY, int rightDownX, int rightDownY)
		:value(value), ID(++ID_dispatcher), topLeftPoint({ leftUpX,leftUpY }), bottomRightPoint({ rightDownX,rightDownY }) {}
	unsigned getID() const { return ID; }
	MAT getMat() { return value; }
	void setMat(const MAT value) { this->value = value; }
	std::pair<int, int> getTopLeftPoint() const { return topLeftPoint; }
	std::pair<int, int> getTopRightPoint() const { return { bottomRightPoint.first,topLeftPoint.second }; }
	std::pair<int, int> getBottomRightPoint() const { return bottomRightPoint; }
	std::pair<int, int> getBottomLeftPoint() const { return { topLeftPoint.first,bottomRightPoint.second }; }
	void setTopLeftPoint(int x, int y) { topLeftPoint.first = x; topLeftPoint.second = y; }
	void setBottomRightPoint(int x, int y) { bottomRightPoint.first = x; bottomRightPoint.second = y; }

};

class LayerStorage
{
private:
	std::deque<Layer> layers;
	int layerLevel = 0;
	auto findLayerByID(unsigned layerID)->std::deque<Layer>::iterator;
public:
	LayerStorage() = default;
	int getLayerLevel() const { return layerLevel; }
	void addLayerAsTop(MAT Mat);
	void addLayerAsTop(Layer &layer);
	void addLayerAsBottom(MAT Mat);
	void addLayerAsBottom(Layer &layer);
	void addLayerAfter(MAT Mat, int index);
	void addLayerAfter(MAT Mat, Layer &layer);
	void deleteLayer(Layer &layer);
	void deleteLayer(unsigned layerID);
	void moveLayerUp(Layer &layer);
	void moveLayerUp(int index);
	void moveLayerUpByID(unsigned layerID);
	void moveLayerDown(Layer &layer);
	void moveLayerDown(int index);
	void moveLayerDownByID(unsigned layerID);
	void moveLayerTo(int index, int targetIndex);
	void moveLayerTo(Layer &layer, Layer &targetLayer);
	void moveLayerToByID(unsigned layerID, unsigned targetLayerID);
	void setLayerSizeAsImageSize(int index);
	void setLayerSizeAsImageSize(Layer &layer);
	void setLayerSizeAsImageSizeByID(unsigned layerID);
	//alpha( [0,1] )是front图层的权重
	void mergeLayers(int frontIndex, int backIndex, double blendAlpha);
	void mergeLayers(Layer &frontLayer, Layer &backLayer, double blendAlpha);
	void mergeLayersByID(unsigned frontLayerID, unsigned backLayerID, double blendAlpha);

	Layer& front() { return layers.front(); }
	Layer& back() { return layers.back(); }

	auto begin()->std::deque<Layer>::iterator { return layers.begin(); }
	auto end()->std::deque<Layer>::iterator { return layers.end(); }
	auto rbegin()->std::deque<Layer>::reverse_iterator { return layers.rbegin(); }
	auto rend()->std::deque<Layer>::reverse_iterator { return layers.rend(); }

	Layer& operator[](int i);
};

struct Trace
{
	MAT traceValue;
	unsigned traceLayerID = 0;
	bool isDeleted = false;
	Trace(const MAT value, const unsigned layerID) :traceValue(value), traceLayerID(layerID) {}
};

class TraceStack
{
private:
	enum { MAX_TRACES = 5 };
	std::vector<Trace> traces;
public:
	TraceStack() = default;
	void push(MAT changedMat, unsigned layerID);
	void pop()
	{
		traces.pop_back();
	}
	Trace top();
};

class ImageProcess
{


	//保存MAX_TRACES个mat,用于撤销
	TraceStack Traces;
public:

	//TODO:支持多个选择区域
	//std::vector<ImagePartial> SelectedParts;

	LayerStorage Layers;

	//撤销上一次的修改
	void revertChange();



	static void GaussianBlur(ImageProcess &process, Layer &layer, double strength);
	static void Sculpture(ImageProcess &process, Layer &layer);
};

#endif