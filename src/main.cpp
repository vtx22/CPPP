#include <SFML/Graphics.hpp>
#include "cppp.hpp"

#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include <thread>
#include <chrono>
using namespace std::chrono_literals;

int main()
{
   sf::ContextSettings settings;
   settings.antialiasingLevel = 16;
   sf::RenderWindow window(sf::VideoMode(1850, 990), "CPPP - CPlusPlusPlotter", sf::Style::Close, settings);

   CPPP plotter(&window);

   std::vector<float> data;
   std::vector<float> dataX;

   int cnt = 0;
   /*
   float stepSize = 2 * M_PI / 8000;

   for (int i = 0; i < 4 * M_PI / stepSize; i++)
   {
      data.push_back(sin(i * stepSize));
   }
   */
   while (window.isOpen())
   {
      sf::Event event;
      while (window.pollEvent(event))
      {
         if (event.type == sf::Event::Closed)
         {
            window.close();
         }
      }

      float stepSize = 2 * M_PI / 750;
      /*
      if (cnt > 4 * M_PI / stepSize)
      {
         data.erase(data.begin());
      }
      */

      data.push_back(sin(stepSize * cnt) * cnt);
      dataX.push_back(cnt);

      window.clear();

      plotter.addPlot(80);
      plotter.setTitle("Sine Wave");
      plotter.setPlotMode(BOX_PLOT);
      plotter.newDataset(dataX, data, LINE, sf::Color::Red);
      plotter.showPlot();

      window.display();
      cnt++;
      // std::this_thread::sleep_for(1ms);
   }

   return 0;
}