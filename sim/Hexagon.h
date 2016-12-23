/*
 * Praise the lord a Hexagon class :) 
 * (Or praise Alex, or Lukas, or both)
 */

#ifndef _HEXAGON_H_
#define _HEXAGON_H_

#include <vector>

#include <SFML/Graphics.hpp>

class Hexagon : public sf::Shape
{
public:
	Hexagon();
	Hexagon(float radius);

	inline std::size_t  getPointCount(void)  const override;
	sf::Vector2f getPoint(std::size_t index) const override;
	sf::Vector2f Hexagon::getSizeFactors() const;
	float getRadius(void) const;

	float width(void) const { return m_width; };
	float height(void) const { return m_height; };

	~Hexagon() override;

private:

	std::vector<sf::Vector2f> m_cornerOffsets;
	float m_radius;
	float m_width, m_height;
};

#endif // _HEXAGON_H_