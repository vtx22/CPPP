#ifndef CPPP_HPP
#define CPPP_HPP
#pragma once

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <vector>
#include <math.h>
#include <string>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>
#include <iostream>
using namespace std::chrono_literals;

enum PLOT_MODE
{
   XY_ONLY = 0,
   XY_ONLY_CZERO,
   BOX_PLOT

} typedef PLOT_MODE;

enum TICK_MODE
{
   CENTER = 0,
   INNER,
   OUTER
} typedef TICK_MODE;

enum LINE_TYPE
{
   SCATTER = 0,
   LINE,
   AREA,

} typedef LINE_TYPE;

enum GRID_MODE
{
   NONE = 0,
   SOLID,
   DASHED
} typedef GRID_MODE;

struct PLOT_DATA
{
   std::vector<float> dataX, dataY;
   LINE_TYPE lineType = LINE_TYPE::SCATTER;
   sf::Color color = sf::Color::Red;
   uint8_t lineWidth = 2, dotSize = 2;

} typedef PLOT_DATA;

class CPPP
{
public:
   CPPP(sf::RenderWindow *window);
   ~CPPP();

   // COORD SYSTEM
   void addPlot(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
   void addPlot(uint8_t border);

   void showPlot();
   void setAxisWeight(uint8_t weight);
   void setPlotMode(PLOT_MODE mode);
   void setGridMode(GRID_MODE mode);
   void setAxisLimits(float minX, float maxX, float minY, float maxY);
   void setAxisLimitsX(float minX, float maxX);
   void setAxisLimitsY(float minY, float maxY);
   void setTitle(std::string title);
   void setFPS(float fps);

   void setMaxSamples(int samples);

   // DATA
   void newDataset(std::vector<float> dataY, LINE_TYPE type, sf::Color color);
   void newDataset(std::vector<float> dataX, std::vector<float> dataY, LINE_TYPE type, sf::Color color);

   // HELPER
   void centerCross();

private:
   void drawAxis(bool dir, uint16_t startX, uint16_t startY, uint8_t width, uint16_t length);
   void drawAxis(bool dir, uint16_t startX, uint16_t startY, uint8_t width, uint16_t length, bool boxAxis);
   void drawTicks(bool dir, uint16_t startX, uint16_t startY, uint16_t length, uint8_t numOfTicks);
   void drawTicks(bool dir, uint16_t startX, uint16_t startY, uint16_t length, uint8_t numOfTicks, bool boxAxis);
   void plotData();
   void plotGrid();
   void calculateAutoscaleLimits();
   void drawLine(sf::Vector2f p1, sf::Vector2f p2, sf::Color color);
   void drawLine(sf::Vector2f p1, sf::Vector2f p2);

   void setFPS(float fps, bool autoFPS);
   void drawFPS();

   float map(float value, float minVal, float maxVal, float newMin, float newMax);
   bool isOutOfBounds(bool x, float value);
   std::string floatToString(float value, uint8_t precision);
   sf::Vector2f functionValue2pixelValue(float x, float y);
   void drawTitle();

   sf::RenderWindow *_window;
   uint16_t _cpX = 0, _cpY = 0, _cpWidth = 100, _cpHeight = 100;
   uint8_t _axisWeight = 2, _numOfTicksX = 5, _numOfTicksY = 3;
   PLOT_MODE _plotMode = PLOT_MODE::XY_ONLY;
   GRID_MODE _gridMode = GRID_MODE::SOLID;
   TICK_MODE _tickMode = TICK_MODE::OUTER;

   float _minX = 0, _minY = 0, _maxX = 1, _maxY = 1;

   bool _autoScaleX = true, _autoScaleY = true;

   std::vector<PLOT_DATA> dataArray;
   sf::Font _labelFont;
   std::string _plotTitle = "";
   std::string _fps = "";
   int _resolution = 700;
   bool _autoFPS = true;
};

#endif // CPPP_HPP