#include "cppp.hpp"

CPPP::CPPP(sf::RenderWindow *window) : _window(window)
{
}

CPPP::~CPPP()
{
}

void CPPP::drawAxis(bool dir, uint16_t startX, uint16_t startY)
{
   sf::RectangleShape axis;
   axis.setSize(sf::Vector2f(100, 100));
   axis.setPosition(sf::Vector2f(100, 100));
   axis.setFillColor(sf::Color::White);
   _window->draw(axis);
}