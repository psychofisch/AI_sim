#pragma once
#include <SFML\Graphics.hpp>
#include "Hexagon.h"

class gameobj : public Hexagon{
public:
	void setTexture(const std::vector<sf::Texture*>& t, int textureId);
	bool isActive();
	void setActive(bool a);

	int getTextureId() const;
private:
	bool m_active;
	int m_textureId;//also acts as the terrain information
};
