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
   sf::RenderWindow window(sf::VideoMode(1600, 900), "CPPP - CPlusPlusPlotter", sf::Style::Close, settings);

   CPPP plotter(&window);

   std::vector<float> data;
   data.push_back(0);
   int cnt = 1;

   while (window.isOpen())
   {
      sf::Event event;
      while (window.pollEvent(event))
      {
         if (event.type == sf::Event::Closed)
            window.close();
      }
      float stepSize = 2 * M_PI / 500;
      if (cnt > 2 * M_PI / stepSize)
      {
         data.erase(data.begin());
      }

      data.push_back(abs(sin(stepSize * cnt)));
      window.clear();
      plotter.addPlot(50);
      plotter.setPlotMode(BOX_PLOT);
      plotter.newDataset(data, LINE, sf::Color::Red);
      plotter.showPlot();

      window.display();
      cnt++;
      std::this_thread::sleep_for(10ms);
   }

   return 0;
}