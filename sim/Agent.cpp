#include "Agent.h"



Agent::Agent()
	:m_targetTile(-1),
	m_pathPos(0),
	m_player(false)
{
}

Agent::~Agent()
{
}

Agent::Agent(const Agent & a)
	:Agent()
{
	Shape::setTexture(a.getTexture());
	setPosition(a.getPosition());

	m_hexgrid = a.m_hexgrid;
}

int Agent::getTarget() const
{
	return m_targetTile;
}

bool Agent::isPlayer() const
{
	return m_player;
}

void Agent::drawPath(sf::RenderWindow * wndw)
{
	Hexagon h;
	h.setFillColor(sf::Color(0,128,255,64));

	for (int i = 0; i < m_path.size(); ++i)
	{
		int pos = m_path[i].x + m_hexgrid->dimensions().y * m_path[i].y;
		h.setPosition((*m_hexgrid)[pos].getPosition());
		wndw->draw(h);
	}
}

void Agent::update(float dt)
{
	if (m_targetTile != -1 && getPosition() != (*m_hexgrid)[m_targetTile].getPosition())
		setTarget(m_targetTile);

	if (m_path.size() > 0 && m_pathPos < m_path.size() - 1)
	{
		m_pathPos++;
		int gridpos = m_path[m_pathPos].x + m_path[m_pathPos].y * m_hexgrid->dimensions().y;
		float rot = angleD((*m_hexgrid)[gridpos].getPosition() - getPosition());

		setPosition((*m_hexgrid)[gridpos].getPosition());
		setRotation(rot - 90.f);
	}


}

bool Agent::setTarget(int t)
{
	if ((*m_hexgrid)[t].getTextureId() == 3)
		return false;

	m_targetTile = t;
	m_path = Astar::findPath(*m_hexgrid, m_hexgrid->getGridCoords(getPosition()), m_hexgrid->getGridCoords(m_targetTile), m_player);
	m_pathPos = 0;

	return true;
}

void Agent::setGrid(HexGrid * hg)
{
	m_hexgrid = hg;
}

void Agent::isPlayer(bool p)
{
	m_player = p;
}

Enemy::Enemy()
	:m_currentWaypoint(-1)
{
}

Enemy::~Enemy()
{
}

void Enemy::addWaypoint(sf::Vector2i p)
{
	m_currentWaypoint = 0;
	m_waypoints.push_back(p);
}

void Enemy::clearWaypoints()
{
	m_waypoints.clear();
}

void Enemy::update(float dt)
{
	if (m_waypoints.size() > 0)
	{
		if (m_hexgrid->getGridCoords(getPosition()) == m_waypoints[m_currentWaypoint])
		{
			m_currentWaypoint++;
			if (m_currentWaypoint >= m_waypoints.size())
				m_currentWaypoint = 0;
		}
		setTarget(m_waypoints[m_currentWaypoint].x + m_waypoints[m_currentWaypoint].y * m_hexgrid->dimensions().y);
	}

	Agent::update(dt);

	sf::Vector2i threatTile = m_hexgrid->getGridCoords(getPosition());
	m_hexgrid->getThreatMap()[threatTile.x + (threatTile.y * m_hexgrid->dimensions().y)] = 1.f;

	int nbInd = roundf(getRotation() / 60.f);
	threatTile += hex_nb_hex[threatTile.x & 1][nbInd];
	//std::cout << m_hexgrid->cubeToHex(hex_nb[nbInd]).x << "|" << m_hexgrid->cubeToHex(hex_nb[nbInd]).y << std::endl;
	//if (m_hexgrid->hexDistance(threatTile, m_hexgrid->getGridCoords(getPosition())) > 1.5f)
	//	std::cout << "mööp!\n";
	//std::cout << m_hexgrid->hexDistance(threatTile, m_hexgrid->getGridCoords(getPosition())) << std::endl;
	m_hexgrid->getThreatMap()[threatTile.x + (threatTile.y * m_hexgrid->dimensions().y)] = .9f;
	
	for (int i = 0; i < 6; ++i)
	{
		//for(int j = 0; j < 6; ++j)
		//sf::Vector2i threatHelper = threatTile + m_hexgrid->cubeToHex(hex_nb[i]);
		int p = (threatTile.x ) & 1;
		sf::Vector2i threatHelper = threatTile + (hex_nb_hex[p][i]);
		int pos = threatHelper.x + (threatHelper.y * m_hexgrid->dimensions().y);
		if(pos > 0 && pos < m_hexgrid->size())
			m_hexgrid->getThreatMap()[pos] += .2f;
	}
}
