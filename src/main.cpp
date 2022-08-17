#include <SFML/Graphics.hpp>
#include "cppp.hpp"

#include <thread>
#include <chrono>
using namespace std::chrono_literals;

int main()
{
   sf::ContextSettings settings;
   settings.antialiasingLevel = 16;
   sf::RenderWindow window(sf::VideoMode(1600, 900), "CPPP - CPlusPlusPlotter", sf::Style::Default, settings);

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

      data.push_back(rand() % 10 + 1);
      window.clear();
      plotter.addPlot(50);
      plotter.setPlotMode(BOX_PLOT);
      plotter.addData(data, LINE, sf::Color::Red);
      plotter.showPlot();

      window.display();
      cnt++;
      std::this_thread::sleep_for(100ms);
   }

   return 0;
}