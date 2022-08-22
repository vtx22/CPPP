#include "cppp.hpp"

CPPP::CPPP(sf::RenderWindow *window) : _window(window)
{
   if (!_labelFont.loadFromFile("./sfml/fonts/cmunrm.ttf"))
   {
      printf("Error: Could not load font!\n");
   }
}

CPPP::~CPPP()
{
}

/*!
Draws an axis (not usable for the two extra axis in boxed mode)
@param dir Direction of the axis (false = x, true = y)
@param startX x value of the axis coordinate (left side for x axis, bottom for y axis)
@param startY y value of the axis coordinate (left side for x axis, bottom for y axis)
@param width Line width of the axis in pixels
@param length Length of the axis in pixels
*/
void CPPP::drawAxis(bool dir, uint16_t startX, uint16_t startY, uint8_t width, uint16_t length)
{
   drawAxis(dir, startX, startY, width, length, false);
}
/*!
Draws an axis
@param dir Direction of the axis (false = x, true = y)
@param startX x value of the axis coordinate (left side for x axis, bottom for y axis)
@param startY y value of the axis coordinate (left side for x axis, bottom for y axis)
@param width Line width of the axis in pixels
@param length Length of the axis in pixels
@param boxAxis If true the axis will be recognized as one of the box axis (right / top) so that no labels will be plotted
*/
void CPPP::drawAxis(bool dir, uint16_t startX, uint16_t startY, uint8_t width, uint16_t length, bool boxAxis)
{
   sf::RectangleShape axis;
   axis.setPosition(sf::Vector2f(startX, startY));
   axis.setFillColor(sf::Color::White);
   axis.setSize(sf::Vector2f(length, width));
   axis.setOrigin(0, _axisWeight / 2);
   axis.setRotation(dir ? -90 : 0);
   _window->draw(axis);

   if (_plotMode == PLOT_MODE::XY_ONLY)
   {
      uint8_t trigRadius = _axisWeight * 3;
      sf::CircleShape arrowTip(trigRadius, 3);
      arrowTip.setOrigin(trigRadius, trigRadius + trigRadius / 2);
      arrowTip.setFillColor(sf::Color::White);
      arrowTip.setPosition(sf::Vector2f(dir ? startX : (startX + length + 15), dir ? (startY - length - 15) : startY));
      arrowTip.setRotation(dir ? 0 : 90);
      _window->draw(arrowTip);

      sf::RectangleShape axisExtend;
      axisExtend.setFillColor(sf::Color::White);
      axisExtend.setSize(sf::Vector2f(15, _axisWeight));
      axisExtend.setRotation(dir ? -90 : 0);
      axisExtend.setOrigin(0, _axisWeight / 2);
      axisExtend.setPosition(sf::Vector2f(dir ? startX : (startX + length), dir ? (startY - length) : startY));
      _window->draw(axisExtend);
   }

   drawTicks(dir, startX, startY, length, dir ? _numOfTicksY : _numOfTicksX, boxAxis);
}

/*!
Adds a fullscreen plot with a specified border around the plot
@param border Plot border to the window outlines in pixels
*/
void CPPP::addPlot(uint8_t border)
{
   uint16_t width = _window->getSize().x;
   uint16_t height = _window->getSize().y;

   addPlot(border, height - border, width - 2 * border, height - 2 * border);
}

/*!
Adds a new plot to the window
@param x x value of the plot coordinate (bottom left)
@param y y value of the plot coordinate (bottom left)
@param width Width of the plot
@param height Height of the plot
*/
void CPPP::addPlot(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
   _cpX = x;
   _cpY = y;
   _cpWidth = width;
   _cpHeight = height;
}

/*!
Show the complete plot
*/
void CPPP::showPlot()
{
   drawAxis(false, _cpX, _cpY, _axisWeight, _cpWidth);
   drawAxis(true, _cpX, _cpY, _axisWeight, _cpHeight);

   if (_plotMode == PLOT_MODE::BOX_PLOT)
   {
      drawAxis(false, _cpX, _cpY - _cpHeight, _axisWeight, _cpWidth, true);
      drawAxis(true, _cpX + _cpWidth, _cpY, _axisWeight, _cpHeight, true);
   }

   plotGrid();
   plotData();
   drawTitle();

   if (_autoFPS)
   {
      auto currentTime = std::chrono::steady_clock::now();
      static auto lastTime = std::chrono::steady_clock::now();
      float fps = 1 / (std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastTime).count() / 1000000.f);
      lastTime = currentTime;
      setFPS(fps, true);
   }
   drawFPS();

   // Fix
   dataArray.clear();
}

/*!
Sets the axis weight in pixels
@param weight Axis weight in pixels
*/
void CPPP::setAxisWeight(uint8_t weight)
{
   _axisWeight = weight;
}

/*!
Sets the plot mode
@param mode Plot mode (e.g. XY_ONLY or BOX_PLOT)
*/
void CPPP::setPlotMode(PLOT_MODE mode)
{
   _plotMode = mode;

   _tickMode = (mode == PLOT_MODE::BOX_PLOT) ? TICK_MODE::INNER : TICK_MODE::OUTER;
}

void CPPP::drawTicks(bool dir, uint16_t startX, uint16_t startY, uint16_t length, uint8_t numOfTicks)
{
   drawTicks(dir, startX, startY, length, numOfTicks, false);
}

void CPPP::drawTicks(bool dir, uint16_t startX, uint16_t startY, uint16_t length, uint8_t numOfTicks, bool boxAxis)
{

   for (uint8_t i = 0; i < numOfTicks; i++)
   {

      uint8_t tickLength = 20;
      if (_tickMode != TICK_MODE::CENTER)
      {
         tickLength /= 2;
      }
      sf::RectangleShape rect(sf::Vector2f(_axisWeight, tickLength));

      uint8_t originX = _axisWeight / 2;
      uint8_t originY = (_tickMode == TICK_MODE::CENTER) ? tickLength / 2 : 0;
      rect.setOrigin(sf::Vector2f(originX, originY));
      rect.setRotation(dir ? 90 : 0);
      if (boxAxis)
      {
         rect.rotate(180);
      }
      if (_tickMode == TICK_MODE::INNER)
      {
         rect.rotate(180);
      }

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

      // Draw Labels
      if (boxAxis)
      {
         continue;
      }

      sf::Text label;
      label.setFont(_labelFont);
      label.setFillColor(sf::Color::White);
      label.setCharacterSize(15);
      float value = 0;

      if (dir)
      {
         value = _minY + i * (_maxY - _minY) / (numOfTicks - 1);
         label.setString(floatToString(value, 2));
         label.setOrigin(sf::Vector2f(label.getGlobalBounds().width, label.getGlobalBounds().height));
         label.setPosition(sf::Vector2f(_cpX - 14, _cpY - i * length / (float)(numOfTicks - 1)));
      }
      else
      {
         value = _minX + i * (_maxX - _minX) / (numOfTicks - 1);
         label.setString(floatToString(value, 2));
         label.setOrigin(sf::Vector2f(label.getGlobalBounds().width / 2, 0));
         label.setPosition(sf::Vector2f(_cpX + i * length / (float)(numOfTicks - 1), _cpY + 14));
      }

      _window->draw(label);
   }
}

void CPPP::centerCross()
{
   uint16_t width = _window->getSize().x;
   uint16_t height = _window->getSize().y;

   drawLine(sf::Vector2f(0, height / 2), sf::Vector2f(width, height / 2));
   drawLine(sf::Vector2f(width / 2, 0), sf::Vector2f(width / 2, height));
}

/*!
Create a new dataset with only Y-Values (X-Values are assumed to be from 0 - Y-Values-Length-1)
@param dataY Vector that contains all Y-Values
@param type Line type (e.g. SCATTER or LINE)
@param color Color of the datset
*/
void CPPP::newDataset(std::vector<float> dataY, LINE_TYPE type, sf::Color color)
{
   std::vector<float> dataX;

   for (int i = 0; i < dataY.size(); i++)
   {
      dataX.push_back(i);
   }

   newDataset(dataX, dataY, type, color);
}

/*!
Create a new dataset with X-Values and their corresponding Y-Values
@param dataX Vector that contains all X-Values
@param dataY Vector that contains all Y-Values
@param type Line type (e.g. SCATTER or LINE)
@param color Color of the datset
*/
void CPPP::newDataset(std::vector<float> dataX, std::vector<float> dataY, LINE_TYPE type, sf::Color color)
{
   // HOTFIX
   dataArray.push_back({dataX, dataY, type, color, 2, 0});
}

void CPPP::plotData()
{

   calculateAutoscaleLimits();

   for (auto array : dataArray)
   {
      int valuesToDelete = array.dataX.size() - _resolution;

      if (array.dataX.size() > _resolution)
      {
         std::vector<float> newDataX, newDataY;
         float stepSize = array.dataX.size() / (float)_resolution;
         for (int i = 0; i < _resolution; i++)
         {
            newDataX.push_back(array.dataX.at((int)(i * stepSize)));
            newDataY.push_back(array.dataY.at((int)(i * stepSize)));
         }
         array.dataX = newDataX;
         array.dataY = newDataY;
      }
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
            float valueX = array.dataX.at(i);
            float valueY = array.dataY.at(i);
            if (valueX < _minX || valueX > _maxX || valueY < _minY || valueY > _maxY)
            {
               continue;
            }
            sf::CircleShape point(array.dotSize, 10);
            point.setOrigin(sf::Vector2f(array.dotSize, array.dotSize));
            point.setFillColor(array.color);
            point.setPosition(functionValue2pixelValue(valueX, valueY));
            _window->draw(point);
            if (i > 0)
            {
               float prevValueX = array.dataX.at(i - 1);
               float prevValueY = array.dataY.at(i - 1);
               if (!isOutOfBounds(true, prevValueX) && !isOutOfBounds(false, prevValueY))
               {
                  drawLine(functionValue2pixelValue(valueX, valueY), functionValue2pixelValue(prevValueX, prevValueY), array.color);
               }
            }
         }
         break;
      }
   }
}

/*!
Sets all axis limits manualle
@param minX X-Axis lower limit
@param maxX X-Axis upper limit
@param minY Y-Axis lower limit
@param maxY Y-Axis upper limit
*/
void CPPP::setAxisLimits(float minX, float maxX, float minY, float maxY)
{
   setAxisLimitsX(minX, maxX);
   setAxisLimitsY(minY, maxY);
}

/*!
Sets the X-Axis limits manualle
@param minX X-Axis lower limit
@param maxX X-Axis upper limit
*/
void CPPP::setAxisLimitsX(float minX, float maxX)
{
   _minX = minX;
   _maxX = maxX;
   _autoScaleX = false;
}

/*!
Sets the Y-Axis limits manualle
@param minY Y-Axis lower limit
@param maxY Y-Axis upper limit
*/
void CPPP::setAxisLimitsY(float minY, float maxY)
{
   _minY = minY;
   _maxY = maxY;
   _autoScaleY = false;
}

void CPPP::calculateAutoscaleLimits()
{
   if (!_autoScaleX && !_autoScaleY)
   {
      return;
   }

   float minX = 0, minY = 0, maxX = 0, maxY = 0;
   for (auto const &datStruct : dataArray)
   {
      if (_autoScaleX)
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
      }
      if (_autoScaleY)
      {
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
   }

   if (_autoScaleX)
   {
      if (minX == maxX)
      {
         minX = minX - 0.1 * abs(minX);
         maxX = maxX + 0.1 * abs(maxX);
      }
      _minX = minX;
      _maxX = maxX;
   }
   if (_autoScaleY)
   {
      if (minY == maxY)
      {
         minY = minY - 0.1 * abs(minY);
         maxY = maxY + 0.1 * abs(maxY);
      }
      _minY = minY;
      _maxY = maxY;
   }
}

/*!
Sets the Grid Mode
@param mode Grid mode (e.g. NONE, SOLID, DASHED)
*/
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
   uint16_t px = round(map(x, _minX, _maxX, _cpX, _cpX + _cpWidth));
   uint16_t py = round(map(y, _minY, _maxY, _cpY, _cpY - _cpHeight));

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

float CPPP::map(float value, float minVal, float maxVal, float newMin, float newMax)
{
   if (value > maxVal)
   {
      return maxVal;
   }
   if (value < minVal)
   {
      return minVal;
   }
   return (value - minVal) * (newMax - newMin) / (maxVal - minVal) + newMin;
}

bool CPPP::isOutOfBounds(bool x, float value)
{
   if (x)
   {
      return (value < _minX || value > _maxX);
   }
   return (value < _minY || value > _maxY);
}

std::string CPPP::floatToString(float value, uint8_t precision)
{
   std::stringstream stream;
   stream << std::fixed << std::setprecision(precision) << value;
   return stream.str();
}

/*!
Sets the plot title which will be shown above the plot
@param title Plot title string
*/
void CPPP::setTitle(std::string title)
{
   _plotTitle = title;
}

void CPPP::drawTitle()
{
   sf::Text title;
   title.setFont(_labelFont);
   title.setFillColor(sf::Color::White);
   title.setCharacterSize(20);
   title.setString(_plotTitle);

   title.setOrigin(sf::Vector2f(title.getGlobalBounds().width / 2, title.getGlobalBounds().height));
   title.setPosition(sf::Vector2f(_cpX + _cpWidth / 2, _cpY - _cpHeight - 20));

   _window->draw(title);
}

/*!
Function to pass the FPS, calculated from your own code
@param fps Current Framerate
*/
void CPPP::setFPS(float fps)
{
   setFPS(fps, false);
}

void CPPP::setFPS(float fps, bool autoFPS)
{
   _autoFPS = autoFPS;
   _fps = floatToString(fps, 0);
}

void CPPP::drawFPS()
{
   sf::Text fps;
   fps.setFont(_labelFont);
   fps.setFillColor(sf::Color::White);
   fps.setCharacterSize(15);
   fps.setString("FPS: " + _fps);

   fps.setOrigin(sf::Vector2f(fps.getGlobalBounds().width, fps.getGlobalBounds().height));
   fps.setPosition(sf::Vector2f(_cpX + _cpWidth, _cpY - _cpHeight - 20));

   _window->draw(fps);
}

/*!
Sets the amount of samples / datapoints taken from the passed dataset for plotting.
If the dataset contains less samples than the max samples all points will be plotted!
@param samples Maximum samples for every datset
*/
void CPPP::setMaxSamples(int samples)
{
   if (samples <= 0)
   {
      _resolution = 100;
   }
   _resolution = samples;
}