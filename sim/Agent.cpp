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

	m_state[State::isAlive] = true;
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

	if (m_targetTile > 0)
	{
		h.setFillColor(sf::Color(255, 0, 255, 128));
		h.setPosition((*m_quadgrid)[m_targetTile].getPosition());
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

	sf::Vector2i iPos = m_quadgrid->getGridCoords(getPosition());

	//*** stats calc
	if (m_aliveTick % 1 == 0)
	{
		m_stats[Stats::thirst] += 2;
		m_stats[Stats::hunger] += 1;
	}
	
	Resource currentResource = m_quadgrid->getResource(iPos);
	if (currentResource > Resource::Empty && currentResource < Resource::RESOURCE_SIZE)
		m_stats[currentResource] -= 10;

	m_stats[Stats::health] = clamp(m_stats[Stats::health]);
	for (int i = 1; i < Stats::STATS_SIZE; ++i)//start at 1 because health is does not affect health
	{
		m_stats[i] = clamp(m_stats[i]);
		if (m_stats[i] >= 100)
		{
			m_stats[0] -= 5;//if any stat is higher than 100 -> lower health
			break;			//dont lower health faster when more than one stat is cirtical
		}
	}
	//*** sc

	//state set
	if (m_stats[Stats::thirst] > 50)
	{
		m_state[State::isThirsty] = true;
	}
	//*** ss

	//survival instinct
	i_think();
	/*
	for (int s = 1; s < Stats::STATS_SIZE; ++s)
	{
		if (m_stats[s] > 50 && currentResource != s)
		{
			std::vector<int> fields = m_quadgrid->findResource(static_cast<Resource>(s));
			int minResInd = -1;
			int minDist = INT_MAX;
			for (int i = 0; i < fields.size(); ++i)
			{
				int tmpDist = Astar::findPath(*m_quadgrid, iPos, m_quadgrid->getGridCoords(fields[i])).size();
				if (tmpDist < minDist)
				{
					minResInd = i;
					minDist = tmpDist;
				}
			}

			if (minResInd != -1)
			{
				setTarget(fields[minResInd]);
			}
			else
				std::cout << "no water source found! =(\n";
			
			break; //only do one thing at the time & stats are sorted by priority
		}
	}*/
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

void Agent::i_think()
{
	int limit = 5;//plan max 5 actions ahead
	std::map<State::Attributes, bool> savedState(m_state);
	std::map<State::Attributes, bool> targetState;
	std::vector<State::Action> todoList;

	//set goals
	targetState[State::isAlive] = true;
	if (savedState[State::isThirsty] == true)
	{
		targetState[State::hasWater] = true;
		targetState[State::isThirsty] = false;
	}
	//*** sg

	for (int i = 0; i < 5; ++i)
	{
		todoList.clear();
		//Priorities: drink, eat, sleep, safety
		for (int a = 0; a < State::ACTION_SIZE; ++a)
		{
			ActionNode root(static_cast<State::Action>(a), i);

			std::map<State::Attributes, bool> tryStates(savedState);
			for (int d = 0; d < todoList.size(); ++d)
				State::doAction(todoList[d], tryStates);
			if (mapCompare(targetState, tryStates))
			{
				break;
			}
		}
	}

	for (int i = 0; i < todoList.size(); ++i)
	{
		std::cout << todoList[i];
	}
	std::cout << std::endl;
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

bool State::doAction(Action a, std::map<Attributes, bool>& attributes)
{
	if (a == drink)
	{
		if (attributes[hasWater] == true)
		{
			attributes[isThirsty] = false;
			return true;
		}
		else
			return false;
	}
	else if (a == eat)
	{
		if (attributes[hasFood] == true)
		{
			attributes[isHungry] = false;
			return true;
		}
		else
			return false;
	}
	else if (a == sleep)
	{
		if (attributes[hasBed] == true)
		{
			attributes[isTired] = false;
			return true;
		}
		else
			return false;
	}
	else if (a == openDoor)
	{
		attributes[feelsUnsecure] = true;
		return true;
	}
	else if (a == closeDoor)
	{
		attributes[feelsUnsecure] = false;
		return false;
	}
	else if (a == gotoWater)
	{
		attributes[hasWater] = true;
		return true;
	}
	else if (a == gotoFood)
	{
		attributes[hasFood] = true;
		return true;
	}
	else if (a == gotoBed)
	{
		attributes[hasBed] = true;
		return true;
	}
	else// if (a == suicide)
	{
		attributes[isThirsty] = false;
		attributes[isHungry] = false;
		attributes[isTired] = false;
		attributes[feelsUnsecure] = false;
		attributes[isAlive] = false;
		return true;
	}
}

ActionNode::ActionNode()
	:action(static_cast<State::Action>(0)),
	next(nullptr)
{
}

ActionNode::ActionNode(State::Action a, int depth)
{
	action = a;

	if (action < 0)
		std::cout << "WHAT!?!?!?!!?\n";

	if (depth <= 0)
	{
		next = nullptr;
	}
	else
	{
		next = new ActionNode[State::ACTION_SIZE]();
		for (int i = 0; i < State::ACTION_SIZE; ++i)
		{
			//next[i] = ActionNode(static_cast<State::Action>(i), depth - 1);
			next[i] = ActionNode(static_cast<State::Action>(i), depth - 1);
		}
	}
}

ActionNode::~ActionNode()
{
	//for (int i = 0; i < State::ACTION_SIZE; ++i)
		delete[] next;
}

bool mapCompare(std::map<State::Attributes, bool>& a, std::map<State::Attributes, bool>& b)
{
	int count = 0;
	for (int i = 0; i < State::ATTR_SIZE; ++i)
	{
		State::Attributes iCast = static_cast<State::Attributes>(i);
		if (a.count(iCast) == 1 && b.count(iCast) == 1 && a[iCast] != b[iCast])
		{
			count++;
			return false;
		}
	}

	return (count > 0) ? true : false;
}
