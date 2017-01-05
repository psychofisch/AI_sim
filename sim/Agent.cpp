#include "Agent.h"

Agent::Agent()
	:m_targetTile(-1),
	m_pathPos(0),
	m_player(false),
	m_alive(true),
	m_aliveTick(0),
	m_thinkAheadLimit(5)
{
	for (int i = 0; i < Stats::STATS_SIZE; ++i)
	{
		m_stats[static_cast<Stats>(i)] = 25;
	}
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

int Agent::getThinkAheadLimit() const
{
	return m_thinkAheadLimit;
}

void Agent::drawPath(sf::RenderWindow * wndw)
{
	Block h;
	h.setFillColor(sf::Color(0,128,255,64));

	for (int i = 0; i < m_path.size(); ++i)
	{
		int pos = m_quadgrid->getGridNumber(m_path[i]);
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
		"S " << m_stats[Stats::danger];
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
	if (m_aliveTick % 5 == 0)
	{
		m_stats[Stats::thirst] += 7;
		m_stats[Stats::hunger] += 5;
		m_stats[Stats::fatique] += 1;
	}
	
	Resource currentResource = m_quadgrid->getResource(iPos);
	if (currentResource > Resource::Empty && currentResource < Resource::RESOURCE_SIZE)
	{
		m_stats[static_cast<Stats>(currentResource)] -= 10; //DIRTY! works because Stats and Resource have the same size and order!
		if (currentResource == Resource::Sleep)
			m_stats[health] += 1;
	}

	m_stats[Stats::health] = clamp(m_stats[Stats::health]);
	for (int i = 1; i < Stats::STATS_SIZE; ++i)//start at 1 because health is does not affect health
	{
		Stats iStat = static_cast<Stats>(i);
		m_stats[iStat] = clamp(m_stats[iStat]);
		if (m_stats[iStat] >= 100)
		{
			m_stats[Stats::health] -= 5;//if any stat is higher than 100 -> lower health
			break;			//dont lower health faster when more than one stat is cirtical
		}
	}
	//*** sc

	//state set
	m_state[State::isThirsty] = i_setStat(Stats::thirst, 50);
	m_state[State::isHungry] = i_setStat(Stats::hunger, 50);
	m_state[State::isTired] = i_setStat(Stats::fatique, 50);
	//*** ss

	//state reaction
	if (currentResource == Resource::Water)
	{
		m_state[State::hasWater] = true;

		if (m_currentAction == State::gotoWater || (m_currentAction == State::drink && m_state[State::isThirsty] == false))
			m_currentAction = State::nothing;
	}
	else if (currentResource == Resource::Food)
	{
		m_state[State::hasFood] = true;

		if (m_currentAction == State::gotoFood || (m_currentAction == State::eat && m_state[State::isHungry] == false))
			m_currentAction = State::nothing;
	}
	else if (currentResource == Resource::Sleep)
	{
		m_state[State::hasBed] = true;

		if (m_currentAction == State::gotoBed || (m_currentAction == State::sleep && m_state[State::isTired] == false))
			m_currentAction = State::nothing;
	}
	else 
	{
		m_state[State::hasWater] = false;
		m_state[State::hasFood] = false;
		m_state[State::hasBed] = false;
	}
	//*** sr

	//survival instinct
	if(m_alive && m_aliveTick%2 == 0)
		i_think();

	if (!m_todoList.empty() && m_currentAction == State::nothing)
	{
		m_currentAction = m_todoList.front();
		m_todoList.pop_front();
		if (m_currentAction == State::gotoWater)
		{
			int waterSource = m_quadgrid->findClosestResource(iPos, Resource::Water);
			if (waterSource != -1)
				setTarget(waterSource);
			else
				std::cout << "no water source found! =(\n";
		}
		else if (m_currentAction == State::gotoFood)
		{
			int foodSource = m_quadgrid->findClosestResource(iPos, Resource::Food);
			if (foodSource != -1)
				setTarget(foodSource);
			else
				std::cout << "no food source found! =(\n";
		}
		else if (m_currentAction == State::gotoBed)
		{
			int bed = m_quadgrid->findClosestResource(iPos, Resource::Sleep);
			if (bed != -1)
				setTarget(bed);
			else
				std::cout << "no bed found! =(\n";
		}
		else if (m_currentAction == State::openDoor)
		{
			m_stats[Stats::danger] += 10;
		}
		else if (m_currentAction == State::closeDoor)
		{
			m_stats[Stats::danger] -= 10;
		}
		else if (m_currentAction == State::nothing || m_currentAction == State::drink || m_currentAction == State::eat)
		{
			//happens automagically
		}
		else
		{
			std::cout << "Action " << m_currentAction << " not implemented yet!\n";
		}
	}
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
		if (m_quadgrid->isLocked(m_path[m_pathPos + 1]) == true)
		{
			std::cout << "damn! this door is locked...\n";
			//m_todoList.push_front(m_currentAction);
			m_todoList.push_front(State::openDoor);
			m_currentAction = State::nothing;
			m_quadgrid->lock(m_path[m_pathPos + 1], Terrain::Activity::openDoor);
		}
		else
		{
			if (m_currentAction == State::openDoor)
				m_currentAction = State::nothing;

			m_pathPos++;
			int gridpos = m_quadgrid->getGridNumber(m_path[m_pathPos]);
			float rot = angleD((*m_quadgrid)[gridpos].getPosition() - getPosition());

			setPosition((*m_quadgrid)[gridpos].getPosition());
			setRotation(rot - 90.f);
		}
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

void Agent::setThinkAheadLimit(int lim)
{
	m_thinkAheadLimit = lim;
}

void Agent::clearTodoList()
{
	m_todoList = std::deque<State::Action>();
	m_currentAction = State::nothing;
}

void Agent::i_think()
{
	std::cout << "Current Action: " << m_currentAction << std::endl;
	std::map<State::Attributes, bool> savedState(m_state);
	std::map<State::Attributes, bool> targetState;

	//set goals
	targetState[State::isAlive] = true;
	if (savedState[State::isThirsty] == true)
	{
		targetState[State::hasWater] = true;
		targetState[State::isThirsty] = false;
	}

	if (savedState[State::isHungry] == true)
	{
		targetState[State::hasFood] = true;
		targetState[State::isHungry] = false;
	}

	if (savedState[State::isTired] == true)
	{
		targetState[State::hasBed] = true;
		targetState[State::isTired] = false;
	}

	if (savedState[State::feelsUnsecure] == true)
	{
		targetState[State::feelsUnsecure] = false;
	}
	//*** sg

	//find actions
	std::vector<State::Action> actions;
	bool found = false;
	for (int i = 1; i <= m_thinkAheadLimit; ++i)//thinking zero steps forward would make no sense
	{
		for (int a = 0; a < State::ACTION_SIZE; ++a)
		{
			ActionNode root(static_cast<State::Action>(a), i, nullptr);
			std::map<State::Attributes, bool> tryStates(savedState);
			found = i_treeLook(&root, tryStates, targetState, actions);
			if (found)
				break;
		}
		if (found)
			break;
	}

	if (found)
	{
		m_todoList = std::deque<State::Action>();
		//std::reverse(actions.begin(), actions.end());
		std::cout << "Actions to do: ";
		for (int i = 0; i < actions.size(); ++i)
		{
			m_todoList.push_front(static_cast<State::Action>(actions[i]));
			std::cout << actions[i] << ">";
		}
		std::cout << "!\n";
	}
	else
		std::cout << "ay em to stupit for dis...\n";
	//*** find actions
}

bool Agent::i_treeLook(ActionNode * an, std::map<State::Attributes, bool> tryStates, std::map<State::Attributes, bool> targetState, std::vector<State::Action>& path)
{
	//std::cout << an->action;
	if (State::doAction(an->action, tryStates) == false)
	{
		//std::cout << "X\n";
		return false;
	}

	if (an->next.size() == 0)
	{
		bool debug = mapCompare(targetState, tryStates);
		if (debug)
		{
			path.push_back(an->action);
			//std::cout << " -> WOOP!\n";
		}
		//else
			//std::cout << "|\n";
		return debug;
	}

	for (int i = 0; i < State::ACTION_SIZE; ++i)
	{
		std::map<State::Attributes, bool> tmpState(tryStates);
		if (i_treeLook(&an->next[i], tmpState, targetState, path))
		{
			path.push_back(an->action);
			return true;
		}
	}

	return false;
}

bool Agent::i_setStat(Stats s, int limit)
{
	if (m_stats[s] > limit)
	{
		return true;
	}
	else if (m_stats[s] == 0)
	{
		return false;
	}
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
		setTarget(m_quadgrid->getGridNumber(m_waypoints[m_currentWaypoint]));
	}

	Agent::update(dt);
}

bool State::doAction(Action a, std::map<Attributes, bool>& attributes)
{
	if (a == nothing)
	{
		return true;
	}
	else if (a == drink)
	{
		if (attributes[hasWater] == true)
		{
			attributes[isThirsty] = false;
		}
		else
			return false;
	}
	else if (a == eat)
	{
		if (attributes[hasFood] == true)
		{
			attributes[isHungry] = false;
		}
		else
			return false;
	}
	else if (a == sleep)
	{
		if (attributes[hasBed] == true)
		{
			attributes[isTired] = false;
		}
		else
			return false;
	}
	else if (a == openDoor)
	{
		attributes[feelsUnsecure] = true;
	}
	else if (a == closeDoor)
	{
		attributes[feelsUnsecure] = false;
	}
	else if (a == gotoWater)
	{
		attributes[hasWater] = true;
	}
	else if (a == gotoFood)
	{
		attributes[hasFood] = true;
	}
	else if (a == gotoBed)
	{
		attributes[hasBed] = true;
	}
	else if (a == suicide)
	{
		attributes[isThirsty] = false;
		attributes[isHungry] = false;
		attributes[isTired] = false;
		attributes[feelsUnsecure] = false;
		attributes[isAlive] = false;
	}

	return true;
}

ActionNode::ActionNode()
	:action(static_cast<State::Action>(0)),
	parent(nullptr),
	todo(true)
{
}

ActionNode::ActionNode(State::Action a, int depth, ActionNode* p)
{
	action = a;
	parent = p;
	todo = true;

	if (depth <= 1)
	{
		next = std::vector<ActionNode>();
	}
	else
	{
		//next = new ActionNode[State::ACTION_SIZE]();
		next.resize(State::ACTION_SIZE);
		for (int i = 0; i < State::ACTION_SIZE; ++i)
		{
			next[i] = ActionNode(static_cast<State::Action>(i), depth - 1, this);
			//next.push_back(ActionNode(static_cast<State::Action>(i), depth - 1));
		}
	}
}

ActionNode::~ActionNode()
{
}

bool mapCompare(std::map<State::Attributes, bool>& a, std::map<State::Attributes, bool>& b)
{
	int count = 0;
	for (int i = 0; i < State::ATTR_SIZE; ++i)
	{
		State::Attributes iCast = static_cast<State::Attributes>(i);
		if (a.count(iCast) == 1 && b.count(iCast) == 1)
		{
			if (a[iCast] != b[iCast])
				return false;
			count++;
		}
	}

	return (count > 0) ? true : false;
}
