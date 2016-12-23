#pragma once

#include "FileIO.h"
#include "Hexagon.h"
#include "vector2math.h"
#include "Agent.h"
#include "HexGrid.h"

class HexWorld
{
public:
	HexWorld();
	~HexWorld();

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
	Hexagon m_currentTile;
	Hexagon m_threatTile;
	HexGrid m_grid;
	std::vector<sf::Texture*> m_texture;
	sf::RenderWindow* m_window;
	sf::View m_view;
	bool m_step;

	void i_init();
};

