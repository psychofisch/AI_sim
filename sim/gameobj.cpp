#include "gameobj.h"

void gameobj::setTexture(const std::vector<sf::Texture*>& t, int textureId)
{
	m_textureId = textureId;

	if (textureId < t.size())
		Shape::setTexture(t[textureId]);
	else
		Shape::setTexture(nullptr);
}

bool gameobj::isActive()
{
	return m_active;
}

void gameobj::setActive(bool a)
{
	m_active = a;
}

int gameobj::getTextureId() const
{
	return m_textureId;
}
