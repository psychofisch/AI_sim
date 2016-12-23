#include "Block.h"

Block::Block(float size)
{
	m_size = size;
	this->setSize(sf::Vector2f(size, size));
}

float Block::getSize() const
{
	return m_size;
}
