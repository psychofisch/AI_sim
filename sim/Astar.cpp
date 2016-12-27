#include "Astar.h"

Astar::Astar()
{
}


Astar::~Astar()
{
}

std::vector<Astar::Node> Astar::findPath(QuadGrid & grid, const sf::Vector2i start, const sf::Vector2i end, bool player)
{
	std::vector<Node> nodes;

	Node currentNode = end;
	nodes.push_back(currentNode);
	do {
		for (int i = 0; i < nodes.size(); ++i)
		{
			if (currentNode == nodes[i])
				nodes[i].todo = false;
		}

		for (int i = 0; i < 4; ++i)
		{
			Node tmp(currentNode + quad_nb[i]);
			if (tmp < sf::Vector2i(0, 0) || tmp > grid.dimensions() - 1)
				continue;

			int terrain = grid.getTerrain(grid[tmp.x + tmp.y * grid.dimensions().y].terrainId);
			if (terrain == 0)
				continue;

			tmp.todo = true;
			tmp.costs = QuadGrid::quadDistance(start, tmp) + terrain;
			tmp.prev = currentNode;

			int prevCosts = 0;
			prevCosts = currentNode.costs - QuadGrid::quadDistance(start, currentNode);
			tmp.costs += prevCosts;

			bool existing = false;
			int existingIndex = -1;
			for (int i = 0; i < nodes.size(); ++i)
			{
				if (tmp == nodes[i])
				{
					existingIndex = i;
					existing = true;
					break;
				}
			}

			if (existing)
			{
				if (tmp.costs < nodes[existingIndex].costs)
				{
					nodes[existingIndex].costs = tmp.costs;
					nodes[existingIndex].prev = tmp.prev;
				}
			}
			else
				nodes.push_back(tmp);
		}

		int minCosts = INT_MAX;
		int minIndex = -1;
		for (int i = 0; i < nodes.size(); ++i)
		{
			if (nodes[i].todo == true && nodes[i].costs < minCosts)
			{
				minIndex = i;
				minCosts = nodes[i].costs;
			}
		}

		if (minIndex == -1)
		{
			std::cout << "CRITICAL ERROR IN A*!\n";
			return std::vector<Node>();
		}

		currentNode = nodes[minIndex];
	} while (currentNode != start);

	std::vector<Node> path;
	path.push_back(currentNode);
	Node tmp = currentNode;
	do {
		for (int i = 0; i < nodes.size(); ++i)
		{
			if (nodes[i] == tmp.prev)
			{
				path.push_back(nodes[i]);
				tmp = nodes[i];
				break;
			}
		}
	} while (tmp != end);

	return path;
}
