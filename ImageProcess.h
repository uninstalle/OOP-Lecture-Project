#ifndef _IMAGE_PROCESS_H
#define _IMAGE_PROCESS_H
#include <deque>
#include <vector>
#include "MAT.h"

//字体，枚举的值与CV内部提供的值保持一致
enum FondFace {
	CV_FONT_HERSHEY_SIMPLEX = 0,		//正常体
	CV_FONT_HERSHEY_TRIPLEX = 4,	    //加粗黑体
	CV_FONT_HERSHEY_SCRIPT_SIMPLEX = 6, //花体
};

//图元类型
enum ElementType {
	ELLIPSE = 0,
	LINE1 = 1,		//斜率<0的直线
	LINE2 = 2,		//斜率>0的直线
	TRIANGLE = 3,
	RECTANGULAR = 4
};

/*
//放缩时围绕哪个点
enum MovablePoint {
TOP_LEFT = 1,
TOP = 2,
TOP_RIGHT = 3,
RIGHT = 4,
BOTTOM_RIGHT = 5,
BOTTON = 6,
BOTTOM_LEFT = 7,
LEFT = 8
};

*/

//使用一个静态类储存当前画笔的颜色，线宽，在创建对应图元时将信息存入对应图元图层,cv暂不提供笔触，硬度等参数
static class penParameter {
private:
	static double B;
	static double G;
	static double R;
	static int size;	//线宽，-1为实心
	static double scale;
	static FondFace face;

public:
	static void setPenColor(double b, double g, double r) { B = b; G = g; R = r; }
	static double getPenColorB() { return B; }
	static double getPenColorG() { return G; }
	static double getPenColorR() { return R; }
	static void setPenSize(int s) { size = s; }
	static int getPenSize() { return size; }
	static void setPenScale(double s) { scale = s; }
	static double getPenScale() { return scale; }
	static void setPenFace(FondFace f) { face = f; }
	static FondFace getPenFace() { return face; }
};

//储存额外信息的类，分为共用，图元，文字
class LayerAttachment {
private:

	//共用
	double B;
	double G;
	double R;

	//图元
	ElementType eType;
	int size;

	//文字
	FondFace fondFace;		//字体
	double fondScale;	//字号
	int fondThickness;	//字体粗细
	std::string text;

public:
	//共用
	LayerAttachment(){}
	void setPenColor(double b, double g, double r) { B = b; G = g; R = r; }
	double getPenColorB() const { return B; }
	double getPenColorG() const { return G; }
	double getPenColorR() const { return R; }

	//图元
	LayerAttachment(ElementType type, int s, double b, double g, double r) :eType(type), size(s), B(b), G(g), R(r) {  }
	void setPenSize(int s) { size = s; }
	int getPenSize() const { return size; }
	ElementType gettype() const { return eType; }

	//文字
	LayerAttachment(std::string s, double b, double g, double r, FondFace face, double scale, int thickness)
		:text(s), B(b), G(g), R(r), fondFace(face), fondScale(scale), fondThickness(thickness) { }
	void setThickness(int s) { fondThickness = s; }
	int getThickness() const { return fondThickness; }
	void setFace(FondFace f) { fondFace = f; }
	FondFace getFace() const { return fondFace; }
	void setScale(double s) { fondScale = s; }
	double getScale() const { return fondScale; }
	void setText(const std::string s) { text = s; }
	std::string getText() const { return text; }
};

class Layer
{
protected:
	/*
		MAT在图元/文字图层与光栅化图层中具有不同的行为特征。
		在图元/文字图层中，一些支持图元/文字图层，改变参数（颜色，大小，位置等）的方法会修改图层中对应的参数，然后重新绘制一遍MAT，
	此MAT仅用于每次改变图元/文字之后的显示。并且不支持局部改变，滤镜等一系列方法。
		而在光栅化图层中，方法将会直接修改MAT。
	*/
	MAT value;
	unsigned ID;
	//利用左上点和右下点标记图层的大小.图层大小只影响绘制时的大小,不影响该图层的value的矩阵尺寸
	std::pair<int, int> topLeftPoint, bottomRightPoint;

	LayerAttachment attachment;

	//图层的属性
	unsigned property = DRAW | RASTERIZED;

	enum LayerProperty
	{
		DRAW = 0x1, //需要绘制
		RASTERIZED = 0x2, //光栅化
		PRIMITIVE = 0x4, //含有图元
		TEXT = 0x8 //含有文字
	};
	using layer_enum = unsigned;

	static int ID_dispatcher;
public:
	Layer(const MAT value);
	//普通图层构造
	Layer(const MAT value, int leftUpX, int leftUpY, int rightDownX, int rightDownY)
		:value(value), ID(++ID_dispatcher), topLeftPoint({ leftUpX,leftUpY }), bottomRightPoint({ rightDownX,rightDownY }) {}
	//图元图层构造
	Layer(ElementType e, int leftUpX, int leftUpY, int rightDownX, int rightDownY);
	//文字图层构造
	//std::string s, int x, int y, double b, double g, double r, FondFace face, double scale, int thickness
	Layer(std::string s, int x, int y);

	unsigned getID() const { return ID; }
	MAT getMat();
	void setMat(const MAT value) { this->value = value; }
	std::pair<int, int> getTopLeftPoint() const { return topLeftPoint; }
	std::pair<int, int> getTopRightPoint() const { return { bottomRightPoint.first,topLeftPoint.second }; }
	std::pair<int, int> getBottomRightPoint() const { return bottomRightPoint; }
	std::pair<int, int> getBottomLeftPoint() const { return { topLeftPoint.first,bottomRightPoint.second }; }
	void setTopLeftPoint(int x, int y) { topLeftPoint.first = x; topLeftPoint.second = y; }
	void setBottomRightPoint(int x, int y) { bottomRightPoint.first = x; bottomRightPoint.second = y; }
	LayerAttachment& getLayerAttachment() { return attachment; }

	bool checkProperty(layer_enum property) const { return this->property&property; }
	void enableProperty(layer_enum property) { this->property |= property; }
	void disableProperty(layer_enum property) { this->property &= ~property; }

	//光栅化图层
	void rasterizeLayer(Layer& layer) {
		layer.enableProperty(RASTERIZED);
		layer.disableProperty(PRIMITIVE);
		layer.disableProperty(TEXT);
	}
	virtual ~Layer() = default;

};

class LayerStorage
{
private:
	std::deque<Layer> layers;
	int layerLevel = 0;
	auto findLayerByID(unsigned layerID)->std::deque<Layer>::iterator;
	//TODO 图层的属性
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

	//滤镜
	static void GaussianBlur(ImageProcess &process, Layer &layer, double strength);				//高斯模糊
	static void Sculpture(ImageProcess &process, Layer &layer);									//浮雕
	static void NostalgicHue(ImageProcess &process, Layer &layer);								//怀旧
	static void StrongLight(ImageProcess &process, Layer &layer);								//强光
	static void DarkTown(ImageProcess &process, Layer &layer, double DarkDegree);				//暗调
	static void Feather(ImageProcess &process, Layer &layer, double VagueRatio);				//羽化
	static void Mosaic(ImageProcess &process, Layer &layer, int size);							//马赛克
	static void Diffusion(ImageProcess &process, Layer &layer);									//扩散（毛玻璃）
	static void Wind(ImageProcess &process, Layer &layer, int strength);						//风

	
	
	//图元图层操作
	static void drawPrimitive(MAT& src, ElementType e, int leftUpX, int leftUpY, int rightDownX, int rightDownY, int size, double B, double G, double R);
	static void movePrimitive(ImageProcess &process, Layer &layer, int dx, int dy);		//移动图元图层
	static void scalePrimitive(ImageProcess &process, Layer &layer, int leftUpdx, int leftUpdy, int RightDowndx, int RightDowndy);
	static void changePrimitiveColor(ImageProcess &process, Layer &layer, double B, double G, double R);
	static void changePrimitivePenSize(ImageProcess &process, Layer &layer, int size);

	//文字图层操作
	static void drawText(MAT& src, std::string s, int leftUpX, int leftUpY, int rightDownX, int rightDownY, int size, int scale, FondFace face, double B, double G, double R);
	static void changeTextColor(ImageProcess &process, Layer &layer, double B, double G, double R);
	static void ImageProcess::changeTextThickness(ImageProcess &process, Layer &layer, int thickness);
	static void ImageProcess::changeTextScale(ImageProcess &process, Layer &layer, int scale);
	static void ImageProcess::changeTextFace(ImageProcess &process, Layer &layer, FondFace face);
	static void ImageProcess::moveText(ImageProcess &process, Layer &layer, int dx, int dy);
	static void ImageProcess::rewriteText(ImageProcess &process, Layer &layer, std::string t);
	
};

#endif