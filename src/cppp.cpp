#include "cppp.hpp"

#include <iostream>

CPPP::CPPP(sf::RenderWindow *window) : _window(window)
{
}

CPPP::~CPPP()
{
}
/*!
Draws an axis
@param dir Direction of the axis (false = x, true = y)

*/
void CPPP::drawAxis(bool dir, uint16_t startX, uint16_t startY, uint8_t width, uint16_t length)
{
   sf::RectangleShape axis;
   axis.setOrigin(0, _axisWeight / 2);
   axis.setPosition(sf::Vector2f(startX, startY));
   axis.setFillColor(sf::Color::White);
   axis.setSize(sf::Vector2f(length, width));
   axis.setRotation(dir ? -90 : 0);
   _window->draw(axis);

   if (_plotMode == PLOT_MODE::XY_ONLY)
   {
      uint8_t trigRadius = _axisWeight * 3;
      sf::CircleShape arrowTip(trigRadius, 3);
      arrowTip.setOrigin(trigRadius, trigRadius + trigRadius / 2);
      arrowTip.setFillColor(sf::Color::White);
      arrowTip.setPosition(sf::Vector2f(dir ? startX : (startX + length), dir ? (startY - length) : startY));
      arrowTip.setRotation(dir ? 0 : 90);
      _window->draw(arrowTip);
   }

   drawTicks(dir, startX, startY, length, dir ? _numOfTicksY : _numOfTicksX);
}

void CPPP::addPlot(uint8_t border)
{
   uint16_t width = _window->getSize().x;
   uint16_t height = _window->getSize().y;

   addPlot(border, height - border, width - 2 * border, height - 2 * border);
}

void CPPP::addPlot(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
   _cpX = x;
   _cpY = y;
   _cpWidth = width;
   _cpHeight = height;
}

void CPPP::showPlot()
{
   drawAxis(false, _cpX, _cpY, _axisWeight, _cpWidth);
   drawAxis(true, _cpX, _cpY, _axisWeight, _cpHeight);

   if (_plotMode == PLOT_MODE::BOX_PLOT)
   {
      drawAxis(false, _cpX, _cpY - _cpHeight, _axisWeight, _cpWidth);
      drawAxis(true, _cpX + _cpWidth, _cpY, _axisWeight, _cpHeight);
   }

   plotGrid();
   plotData();
}

void CPPP::setAxisWeight(uint8_t weight)
{
   _axisWeight = weight;
}
void CPPP::setPlotMode(PLOT_MODE mode)
{
   _plotMode = mode;
}

void CPPP::drawTicks(bool dir, uint16_t startX, uint16_t startY, uint16_t length, uint8_t numOfTicks)
{

   for (uint8_t i = 0; i < numOfTicks; i++)
   {

      uint8_t tickLength = 10;
      if (_tickMode != TICK_MODE::CENTER)
      {
         tickLength /= 2;
      }
      sf::RectangleShape rect(sf::Vector2f(_axisWeight, tickLength));

      uint8_t originX = _axisWeight / 2;
      uint8_t originY = (_tickMode == TICK_MODE::CENTER) ? tickLength / 2 : 0;
      rect.setOrigin(sf::Vector2f(originX, originY));
      rect.setRotation(dir ? 90 : 0);

      rect.setFillColor(sf::Color::White);
      if (dir)
      {
         rect.setPosition(sf::Vector2f(startX, startY - i * length / (float)(numOfTicks - 1)));
      }
      else
      {
         rect.setPosition(sf::Vector2f(startX + i * length / (float)(numOfTicks - 1), startY));
      }

      _window->draw(rect);
   }
}

void CPPP::centerCross()
{
   uint16_t width = _window->getSize().x;
   uint16_t height = _window->getSize().y;

   drawLine(sf::Vector2f(0, height / 2), sf::Vector2f(width, height / 2));
   drawLine(sf::Vector2f(width / 2, 0), sf::Vector2f(width / 2, height));
}

void CPPP::newDataset(std::vector<float> data, LINE_TYPE type, sf::Color color)
{
   std::vector<float> dataX;

   for (int i = 0; i < data.size(); i++)
   {
      dataX.push_back(i);
   }
   // HOTFIX
   if (dataArray.size() > 0)
   {
      dataArray.erase(dataArray.begin());
   }
   dataArray.push_back({dataX, data, type, color});
}

void CPPP::plotData()
{
   if (_autoScale)
   {
      calculateAutoscaleLimits();
   }

   for (auto const &array : dataArray)
   {
      switch (array.lineType)
      {
      default:
      case LINE_TYPE::SCATTER:
         for (int i = 0; i < array.dataX.size(); i++)
         {
            sf::CircleShape point(array.dotSize, 10);
            point.setOrigin(sf::Vector2f(array.dotSize, array.dotSize));
            point.setFillColor(array.color);
            point.setPosition(functionValue2pixelValue(array.dataX.at(i), array.dataY.at(i)));
            _window->draw(point);
         }
         break;
      case LINE_TYPE::LINE:
         for (int i = 0; i < array.dataX.size(); i++)
         {
            sf::CircleShape point(array.dotSize, 10);
            point.setOrigin(sf::Vector2f(array.dotSize, array.dotSize));
            point.setFillColor(array.color);
            point.setPosition(functionValue2pixelValue(array.dataX.at(i), array.dataY.at(i)));
            _window->draw(point);

            if (i > 0)
            {
               drawLine(functionValue2pixelValue(array.dataX.at(i), array.dataY.at(i)), functionValue2pixelValue(array.dataX.at(i - 1), array.dataY.at(i - 1)), array.color);
            }
         }
         break;
      }
   }
}

void CPPP::setAxisLimits(float minX, float maxX, float minY, float maxY)
{
   _minX = minX;
   _maxX = maxX;
   _minY = minY;
   _maxY = maxY;
   _autoScale = false;
}

void CPPP::calculateAutoscaleLimits()
{
   float minX = 0, minY = 0, maxX = 0, maxY = 0;
   for (auto const &datStruct : dataArray)
   {
      for (auto const &dat : datStruct.dataX)
      {
         if (dat < minX)
         {
            minX = dat;
         }
         if (dat > maxX)
         {
            maxX = dat;
         }
      }
      for (auto const &dat : datStruct.dataY)
      {
         if (dat < minY)
         {
            minY = dat;
         }
         if (dat > maxY)
         {
            maxY = dat;
         }
      }
   }

   _minX = minX;
   _minY = minY;
   _maxX = maxX;
   _maxY = maxY;
}

void CPPP::setGridMode(GRID_MODE mode)
{
   _gridMode = mode;
}

void CPPP::plotGrid()
{
   if (_gridMode == GRID_MODE::NONE)
   {
      return;
   }

   if (_gridMode == GRID_MODE::SOLID)
   {
      float stepX = _cpWidth / (float)(_numOfTicksX - 1);
      float stepY = _cpHeight / (float)(_numOfTicksY - 1);
      sf::Color gridColor = sf::Color::White;
      gridColor.a = 30;
      for (uint8_t i = 1; i < _numOfTicksX - 1; i++)
      {
         drawLine(sf::Vector2f(_cpX + i * stepX, _cpY), sf::Vector2f(_cpX + i * stepX, _cpY - _cpHeight), gridColor);
      }
      for (uint8_t i = 1; i < _numOfTicksY - 1; i++)
      {
         drawLine(sf::Vector2f(_cpX, _cpY - i * stepY), sf::Vector2f(_cpX + _cpWidth, _cpY - i * stepY), gridColor);
      }
   }
}

sf::Vector2f CPPP::functionValue2pixelValue(float x, float y)
{
   uint16_t px = _cpX + _cpWidth * x / (_maxX - _minX);
   uint16_t py = _cpY - _cpHeight * y / (_maxY - _minY);

   return sf::Vector2f(px, py);
}

void CPPP::drawLine(sf::Vector2f p1, sf::Vector2f p2, sf::Color color)
{
   sf::Vertex v1 = sf::Vertex(p1);
   v1.color = color;
   sf::Vertex v2 = sf::Vertex(p2);
   v2.color = color;
   sf::Vertex line[] =
       {
           v1,
           v2};

   _window->draw(line, 2, sf::Lines);
}

void CPPP::drawLine(sf::Vector2f p1, sf::Vector2f p2)
{
   drawLine(p1, p2, sf::Color::White);
}