#include "Agent.h"

Agent::Agent()
	:m_targetTile(-1),
	m_pathPos(0),
	m_player(false),
	m_alive(true),
	m_aliveTick(0)
{
	for (int i = 0; i < Stats::STATS_SIZE; ++i)
		m_stats.push_back(25);
	m_stats[Stats::health] = 100;
}

Agent::~Agent()
{
}

Agent::Agent(const Agent & a)
	:Agent()
{
	Shape::setTexture(a.getTexture());
	setPosition(a.getPosition());

	m_quadgrid = a.m_quadgrid;
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
	Block h;
	h.setFillColor(sf::Color(0,128,255,64));

	for (int i = 0; i < m_path.size(); ++i)
	{
		int pos = m_path[i].x + m_quadgrid->dimensions().y * m_path[i].y;
		h.setPosition((*m_quadgrid)[pos].getPosition());
		wndw->draw(h);
	}
}

void Agent::drawStats(sf::RenderWindow * wndw, sf::Text& text)
{
	std::stringstream tmp;
	tmp << ((m_alive) ? "ALIVE" : "DEAD") << std::endl << 
		"H " << m_stats[Stats::health] << std::endl <<
		"T " << m_stats[Stats::thirst] << std::endl <<
		"H " << m_stats[Stats::hunger] << std::endl <<
		"F " << m_stats[Stats::fatique] << std::endl <<
		"S " << m_stats[Stats::safety];
	text.setString(tmp.str());

	if (m_alive == false)
		text.setFillColor(sf::Color::Red);

	wndw->draw(text);
}

void Agent::update(float dt)
{
	if (m_alive == false)
		return;

	int oldTarget = m_targetTile;
	m_aliveTick++;

	//*** stats calc
	if (m_aliveTick % 1 == 0)
	{
		m_stats[Stats::thirst] += 5;
	}
	
	m_stats[Stats::health] = clamp(m_stats[Stats::health]);
	for (int i = 1; i < Stats::STATS_SIZE; ++i)//start at 1 because health is does not affect health
	{
		m_stats[i] = clamp(m_stats[i]);
		if (m_stats[i] >= 100)
		{
			m_stats[0] -= 5;//if any stat is higher than 100 -> lower health
			break;
		}
	}
	//*** sc

	//survival instinct

	//*** si

	//Die
	if (m_stats[Stats::health] <= 0)
		m_alive = false;
	//*** d

	//Moving
	if (m_targetTile != -1 && getPosition() != (*m_quadgrid)[m_targetTile].getPosition())
		setTarget(m_targetTile);

	if (m_path.size() > 0 && m_pathPos < m_path.size() - 1)
	{
		m_pathPos++;
		int gridpos = m_path[m_pathPos].x + m_path[m_pathPos].y * m_quadgrid->dimensions().y;
		float rot = angleD((*m_quadgrid)[gridpos].getPosition() - getPosition());

		setPosition((*m_quadgrid)[gridpos].getPosition());
		setRotation(rot - 90.f);
	}
	//*** m
}

bool Agent::setTarget(int t)
{
	if ((*m_quadgrid)[t].getTextureId() == 3)
		return false;

	m_targetTile = t;
	m_path = Astar::findPath(*m_quadgrid, m_quadgrid->getGridCoords(getPosition()), m_quadgrid->getGridCoords(m_targetTile), m_player);
	m_pathPos = 0;

	return true;
}

void Agent::setGrid(QuadGrid * hg)
{
	m_quadgrid = hg;
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
		if (m_quadgrid->getGridCoords(getPosition()) == m_waypoints[m_currentWaypoint])
		{
			m_currentWaypoint++;
			if (m_currentWaypoint >= m_waypoints.size())
				m_currentWaypoint = 0;
		}
		setTarget(m_waypoints[m_currentWaypoint].x + m_waypoints[m_currentWaypoint].y * m_quadgrid->dimensions().y);
	}

	Agent::update(dt);

	/*
	sf::Vector2i threatTile = m_quadgrid->getGridCoords(getPosition());
	m_quadgrid->getThreatMap()[threatTile.x + (threatTile.y * m_quadgrid->dimensions().y)] = 1.f;

	int nbInd = roundf(getRotation() / 90.f);
	threatTile += quad_nb[nbInd];
	m_quadgrid->getThreatMap()[threatTile.x + (threatTile.y * m_quadgrid->dimensions().y)] = .9f;
	
	for (int i = 0; i < 4; ++i)
	{
		sf::Vector2i threatHelper = threatTile + (quad_nb[i]);
		int pos = threatHelper.x + (threatHelper.y * m_quadgrid->dimensions().y);
		if(pos > 0 && pos < m_quadgrid->size())
			m_quadgrid->getThreatMap()[pos] += .2f;
	}
	*/
}
