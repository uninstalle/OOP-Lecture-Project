#pragma once
#include <vector>

struct vec2d
{
	double x, y;
};

struct HistogramDrawingSet
{
	enum { HISTOGRAM_ALIGN_LEFT, HISTOGRAM_ALIGN_CENTERED };

	int ChartWidth = 100;
	int ChartHeight = 100;
	int ChartWidthOffset = 0;
	int ChartHeightOffset = 0;
	bool isDrawingAxisData = false;
	bool isDrawingBarData = false;
	int AxisDataPrecision = 0;
	int BarDataPrecision = 0;
	bool isDrawingBrokenLine = false;
	int AxisDataAlignment;
};

class QPainter;

class Histogram
{
private:
	double leftOfBars;
	double barsWidth;
	std::vector<double> barsHeight;
	double maximumHeight;
	double maximumDepth;
	void drawBars(int left, int up, int width, int height, QPainter &painter);
	void drawBrokenLines(int x, int y, bool isFirstPoint, QPainter &painter);
	void drawChartData(double data, int width, int height, int precision, QPainter &painter);
public:
	Histogram(std::vector<vec2d> rawData);
	Histogram(std::vector<vec2d> rawData, double barsWidth);
	void drawHistogram(const HistogramDrawingSet settings, QPainter & painter);
};
