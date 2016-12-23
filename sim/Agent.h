#pragma once
#include "gameobj.h"
#include "Astar.h"
#include "QuadGrid.h"

class Agent : public gameobj
{
public:
	Agent();
	~Agent();
	Agent(const Agent& a);

	int getTarget() const;
	bool isPlayer() const;
	void drawPath(sf::RenderWindow* wndw);

	virtual void update(float dt);
	bool setTarget(int t);
	void setGrid(QuadGrid* hg);
	void isPlayer(bool p);
	
protected:
	int m_targetTile;
	bool m_player;
	std::vector<Astar::Node> m_path;
	int m_pathPos;
	QuadGrid* m_hexgrid;
	bool m_alive;
	float	m_health,
		m_thirst,
		m_hunger,
		m_fatique;
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
