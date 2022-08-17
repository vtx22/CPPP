#include <SFML/Graphics.hpp>

#include <cstdint>

class CPPP
{
public:
   CPPP(sf::RenderWindow *window);
   ~CPPP();

   void drawAxis(bool dir, uint16_t startX, uint16_t startY);

private:
   sf::RenderWindow *_window;
};
