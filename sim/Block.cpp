#include "Block.h"

Block::Block(float size)
{
	m_size = size;
	this->setSize(sf::Vector2f(size, size));
	this->setOrigin(sf::Vector2f(size/2.f, size/2.f));
}

float Block::getSize() const
{
	return m_size;
}
