#include "Histogram.h"
#include <algorithm>
#include <QPainter>

std::pair<std::pair<double, double>, std::pair<double, double>> getMinMax(const std::vector<vec2d> &vec)
{
	//first is x's min and max, second is y's min and max.
	std::pair<std::pair<double, double>, std::pair<double, double>> minAndMax;
	minAndMax.first.first = minAndMax.first.second = vec.front().x;
	minAndMax.second.first = minAndMax.second.second = vec.front().y;
	for (auto i : vec)
	{
		if (i.x < minAndMax.first.first) //min x
			minAndMax.first.first = i.x;
		if (i.x > minAndMax.first.second) //max x
			minAndMax.first.second = i.x;
		if (i.y < minAndMax.second.first) //min y
			minAndMax.second.first = i.y;
		if (i.y > minAndMax.second.second) //max y
			minAndMax.second.second = i.y;
	}
	return minAndMax;
}

Histogram::Histogram(std::vector<vec2d> rawData, double barsWidth) :barsWidth(barsWidth)
{
	auto min_max = getMinMax(rawData);
	leftOfBars = min_max.first.first;
	int numOfBars = (min_max.first.second - leftOfBars) / barsWidth + 1; // Left-closed, right-opened interval
	barsHeight.resize(numOfBars);
	for (auto &i : rawData)
	{
		barsHeight[(i.x - leftOfBars) / barsWidth] += i.y;
	}
	maximumHeight = *std::max_element(barsHeight.begin(), barsHeight.end());
	maximumDepth = *std::min_element(barsHeight.begin(), barsHeight.end());
}

Histogram::Histogram(std::vector<vec2d> rawData)
{
	auto min_max = getMinMax(rawData);
	leftOfBars = min_max.first.first;
}


void Histogram::drawHistogram(const HistogramDrawingSet settings, QPainter & painter)
{
	double absDepth = std::abs(maximumDepth);
	//originAxis marks the zero line
	int originAxis = maximumHeight / (maximumHeight + absDepth) *settings.ChartHeight + settings.ChartHeightOffset;
	int currentWidth = settings.ChartWidthOffset, widthOffset = settings.ChartWidth / barsHeight.size();
	double currentWidthData = leftOfBars;
	QPoint lastPoint;
	bool isFirstPoint = true;
	for (auto i : barsHeight)
	{
		int up, height;
		up = (maximumHeight - i) / (maximumHeight - maximumDepth) * settings.ChartHeight + settings.ChartHeightOffset;
		height = i / (maximumHeight - maximumDepth) * settings.ChartHeight;

		drawBars(currentWidth, up, widthOffset, height, painter);
		if (settings.isDrawingBrokenLine)
			drawBrokenLines(currentWidth + widthOffset / 2, up, isFirstPoint, painter);
		isFirstPoint = false;
		if (settings.isDrawingBarData)
			drawChartData(i, currentWidth, up, settings.BarDataPrecision, painter);
		if (settings.isDrawingAxisData)
			drawChartData(currentWidthData, currentWidth, originAxis, settings.AxisDataPrecision, painter);

		//update
		currentWidth += widthOffset;
		currentWidthData += barsWidth;
	}
}

void Histogram::drawBars(int left, int up, int width, int height, QPainter &painter)
{
	QRect bar = { left,up,width,height };
	painter.drawRect(bar);
}
void Histogram::drawBrokenLines(int x, int y, bool isFirstPoint, QPainter &painter)
{
	static QPoint lastPoint;
	if (isFirstPoint)
		lastPoint = { x,y };
	QPoint currentPoint = { x,y };
	painter.drawLine(lastPoint, currentPoint);
	lastPoint = currentPoint;
}
void Histogram::drawChartData(double data, int width, int height, int precision, QPainter &painter)
{
	std::string stdStr = std::to_string(data);
	if (precision > 0) precision++;
	stdStr = stdStr.substr(0, stdStr.size() - 7 + precision);
	QString dataStr = stdStr.c_str();
	painter.drawText(width, height, dataStr);
}