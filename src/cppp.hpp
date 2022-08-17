#include <SFML/Graphics.hpp>

#include <cstdint>
#include <vector>

enum PLOT_MODE
{
   XY_ONLY = 0,
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
   uint8_t lineWidth = 2, dotSize = 5;

} typedef PLOT_DATA;

class CPPP
{
public:
   CPPP(sf::RenderWindow *window);
   ~CPPP();

   // COORD SYSTEM
   void addPlot(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
   void addPlot(uint8_t border);
   void drawAxis(bool dir, uint16_t startX, uint16_t startY, uint8_t width, uint16_t length);
   void showPlot();
   void setAxisWeight(uint8_t weight);
   void setPlotMode(PLOT_MODE mode);
   void setGridMode(GRID_MODE mode);
   void setAxisLimits(float minX, float maxX, float minY, float maxY);

   // DATA
   void addData(std::vector<float> data, LINE_TYPE type, sf::Color color);

   // HELPER
   void centerCross();

private:
   void drawTicks(bool dir, uint16_t startX, uint16_t startY, uint16_t length, uint8_t numOfTicks);
   void plotData();
   void plotGrid();
   void calculateAutoscaleLimits();
   void drawLine(sf::Vector2f p1, sf::Vector2f p2, sf::Color color);
   void drawLine(sf::Vector2f p1, sf::Vector2f p2);
   sf::Vector2f functionValue2pixelValue(float x, float y);

   sf::RenderWindow *_window;
   uint16_t _cpX = 0, _cpY = 0, _cpWidth = 100, _cpHeight = 100;
   uint8_t _axisWeight = 2, _numOfTicks = 5;
   PLOT_MODE _plotMode = PLOT_MODE::XY_ONLY;
   GRID_MODE _gridMode = GRID_MODE::NONE;

   float _minX = 0, _minY = 0, _maxX, _maxY;

   bool _autoScale = true;

   std::vector<PLOT_DATA> dataArray;
};
