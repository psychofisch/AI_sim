#pragma once

#include "FileIO.h"
#include "Block.h"
#include "vector2math.h"
#include "Agent.h"
#include "QuadGrid.h"

class QuadWorld
{
public:
	QuadWorld();
	~QuadWorld();

	void run();
	void loadLevel(const char* path);
	void loadLevel(const char* gridpath, const char* texpath);
	void setRenderWindow(sf::RenderWindow* wndw);

	float tick;
	bool stepMode;

	//static int getGridNumber(const std::vector<gameobj>& grid, sf::Vector2f pos);
	//static sf::Vector2i getGridCoords(std::vector<gameobj>& m_grid, sf::Vector2f pos);

private:
	Agent m_player;
	std::vector<Enemy> m_enemies;
	Block m_currentTile;
	Block m_threatTile;
	QuadGrid m_grid;
	std::vector<sf::Texture*> m_texture;
	sf::RenderWindow* m_window;
	sf::View m_view;
	sf::Font m_HUDFont;
	sf::Text m_HUDText;
	bool m_step;

	void i_init();
};

