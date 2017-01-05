#pragma once
#include "gameobj.h"
#include "vector2math.h"

class Astar;

const sf::Vector2i quad_nb[4] = {
	sf::Vector2i(0, -1), sf::Vector2i(1,  0),
	sf::Vector2i(0, 1), sf::Vector2i(-1, 0)
};

class Terrain : public gameobj {
public:
	int terrainId;
	bool isLocked;
};

enum Resource { Empty = 0, Water, Food, Sleep, Safety, RESOURCE_SIZE };

class QuadGrid
{
public:
	QuadGrid();
	~QuadGrid();

	std::vector<Terrain>& getGrid();
	std::vector<Resource>& getResource();
	void dimensions(const sf::Vector2i& d);
	void initResources();
	bool insertTerrain(int id, int costs);//return true = insertion | false = assign

	//mimic vector behaviour
	Terrain &operator[](int i);
	size_t size() const;
	void push_back(const Terrain& o);
	void reserve(size_t s);

	//const getters
	sf::Vector2i dimensions() const;
	int getGridNumber(sf::Vector2f pos) const;
	sf::Vector2i getGridCoords(sf::Vector2f pos) const;
	sf::Vector2i getGridCoords(int pos) const;
	int getTerrain(int key);					//TODO: make const
	Resource getResource(int index);			//TODO: make const
	Resource getResource(sf::Vector2i pos);		//TODO: make const
	sf::Vector2f getRealCoords(int p);			//TODO: make const
	sf::Vector2f getRealCoords(sf::Vector2i p);	//TODO: make const
	sf::Vector2f getRealCoords(int x, int y);	//TODO: make const
	std::vector<int> findResource(Resource r);
	int findClosestResource(sf::Vector2i pos, Resource r);

	//statics
	static int quadDistance(sf::Vector2i a, sf::Vector2i b);
	/*static sf::Vector2i rotate(sf::Vector2i a, sf::Vector2i b, int t);*/

private:
	sf::Vector2i m_dimension;
	std::vector<Terrain> m_grid;
	std::vector<Resource> m_resources;
	std::map<int, int> m_terrains;
};

