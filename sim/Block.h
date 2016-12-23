#pragma once

#include <SFML\Graphics.hpp>

class Block : public sf::RectangleShape
{
public:
	Block(float size = 5.f);

	float getSize() const;

private:
	float m_size;
};