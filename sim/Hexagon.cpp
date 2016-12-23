#include "Hexagon.h"

Hexagon::Hexagon() : Hexagon(5.0f)
{
}

Hexagon::Hexagon(float radius)
	: m_radius(radius)
{
	// Corner offsets
	m_cornerOffsets.push_back(sf::Vector2f(m_radius * 1, m_radius * 0));
	m_cornerOffsets.push_back(sf::Vector2f(m_radius * 0.5f, m_radius * 0.866f));
	m_cornerOffsets.push_back(sf::Vector2f(m_radius * -0.5f, m_radius * 0.866f));
	m_cornerOffsets.push_back(sf::Vector2f(m_radius * -1, m_radius * 0));
	m_cornerOffsets.push_back(sf::Vector2f(m_radius * -0.5f, m_radius * -0.866f));
	m_cornerOffsets.push_back(sf::Vector2f(m_radius * 0.5f, m_radius * -0.866f));

	m_width = m_radius * 2.0f;
	m_height = m_width * 0.866f; // sqrt(3)/2 yields 0.866 - same for every hexagon

	update();
}

std::size_t Hexagon::getPointCount(void) const
{
	return 6;
}

sf::Vector2f Hexagon::getPoint(std::size_t index) const
{
	return getPosition() + m_cornerOffsets[index];
}

sf::Vector2f Hexagon::getSizeFactors() const
{
	return sf::Vector2f(m_width, m_height);
}

float Hexagon::getRadius(void) const
{
	return m_radius;
}

Hexagon::~Hexagon()
{
}
