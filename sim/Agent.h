#pragma once

#include <sstream>
#include <deque>

#include "gameobj.h"
#include "Astar.h"
#include "QuadGrid.h"

enum Stats { health = 0, thirst, hunger, fatique, danger, STATS_SIZE };
//enum Actions { moveTo = 0, drink, eat, sleep, ActionsSIZE };

class State {
public:
	enum Attributes { isAlive = 0, isThirsty, isHungry, isTired, feelsUnsecure, hasWater, hasFood, hasBed, hasDoor, ATTR_SIZE };
	enum Action { nothing = 0, drink, eat, sleep, openDoor, closeDoor, suicide, gotoWater, gotoFood, gotoBed, gotoDoor, ACTION_SIZE };

	static bool doAction(Action a, std::map<Attributes, bool>& attributes);
};

class ActionNode {
public:
	State::Action action;
	ActionNode* parent;
	std::vector<ActionNode> next;
	bool todo;
	
	ActionNode();
	ActionNode(State::Action a, int depth, ActionNode* p);
	
	~ActionNode();
};

bool mapCompare(std::map<State::Attributes, bool>& a, std::map<State::Attributes, bool>& b);

class Agent : public gameobj
{
public:
	Agent();
	~Agent();
	Agent(const Agent& a);

	int getTarget() const;
	bool isPlayer() const;
	int getThinkAheadLimit() const;
	void drawPath(sf::RenderWindow* wndw);
	void drawStats(sf::RenderWindow* wndw, sf::Text& text);

	virtual void update(float dt);
	bool setTarget(int t);
	void setGrid(QuadGrid* hg);
	void isPlayer(bool p);
	void setThinkAheadLimit(int l);
	void clearTodoList();
	
protected:
	int m_targetTile;
	int m_aliveTick;
	int m_thinkAheadLimit;
	bool m_player;
	std::vector<Astar::Node> m_path;
	int m_pathPos;
	QuadGrid* m_quadgrid;
	bool m_alive;
	std::map<Stats, int> m_stats;
	std::map<State::Attributes, bool> m_state;
	//std::vector<State::Action> m_todoList;
	std::deque<State::Action> m_todoList;
	State::Action m_currentAction;

	void i_think();
	bool i_treeLook(ActionNode* an, std::map<State::Attributes, bool> tryStates,  std::map<State::Attributes, bool> targetState, std::vector<State::Action>& path);
	bool i_setStat(Stats s, int limit);
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