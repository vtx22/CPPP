#include <SFML/Graphics.hpp>
#include "cppp.hpp"

int main()
{
   sf::RenderWindow window(sf::VideoMode(1280, 720), "CPPP - CPlusPlusPlotter");

   CPPP plotter(&window);

   while (window.isOpen())
   {
      sf::Event event;
      while (window.pollEvent(event))
      {
         if (event.type == sf::Event::Closed)
            window.close();
      }

      window.clear();

      plotter.drawAxis(0, 0, 0);

      window.display();
   }

   return 0;
}