#pragma once
#include <vector>
#include <iostream>
#include <SFML\Graphics.hpp>

#include "QuadGrid.h"

inline sf::Vector2i operator-(sf::Vector2i lhs, const int& rhs)
{
	lhs.x -= rhs;
	lhs.y -= rhs;
	return lhs;
}

class Astar
{
public:
	struct Node : public sf::Vector2i{
		int costs;
		bool todo;
		sf::Vector2i prev;

		Node(sf::Vector2i p)
			:costs(0),//TODO: INFINITY and HUGE_VALF also works, INT_AX doesnt, why?
			todo(true),
			prev(-1,-1)
		{
			this->x = p.x;
			this->y = p.y;
		}

		Node(int x, int y)
			:Node(sf::Vector2i(x, y))
		{}

		inline bool operator< (const sf::Vector2i& other)
		{
			return ((this->x < other.x) || (this->y < other.y)) ? true : false;
		}

		inline bool operator> (const sf::Vector2i& other)
		{
			return ((this->x > other.x) || (this->y > other.y)) ? true : false;
		}
	};

	Astar();
	~Astar();

	static std::vector<Node> findPath(QuadGrid& grid, const sf::Vector2i& start, const sf::Vector2i& end, bool player);

private:
};

