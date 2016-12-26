#pragma once

#include <sstream>

#include "gameobj.h"
#include "Astar.h"
#include "QuadGrid.h"

enum Stats { health = 0, thirst, hunger, fatique, safety, STATS_SIZE };

class Agent : public gameobj
{
public:
	Agent();
	~Agent();
	Agent(const Agent& a);

	int getTarget() const;
	bool isPlayer() const;
	void drawPath(sf::RenderWindow* wndw);
	void drawStats(sf::RenderWindow* wndw, sf::Text& text);

	virtual void update(float dt);
	bool setTarget(int t);
	void setGrid(QuadGrid* hg);
	void isPlayer(bool p);
	
protected:
	int m_targetTile;
	int m_aliveTick;
	bool m_player;
	std::vector<Astar::Node> m_path;
	int m_pathPos;
	QuadGrid* m_quadgrid;
	bool m_alive;
	std::vector<int> m_stats;
	/*int	m_health,
		m_thirst,
		m_hunger,
		m_fatique;*/
};

class Enemy : public Agent
{
public:
	Enemy();
	~Enemy();

	void addWaypoint(sf::Vector2i p);
	void clearWaypoints();

	virtual void update(float dt) override;
private:
	std::vector<sf::Vector2i> m_waypoints;
	int m_currentWaypoint;
};
