#pragma once
#include "gameobj.h"
#include "vector2math.h"

/*
const sf::Vector3i hex_nb[6] = {	
	sf::Vector3i(0, -1, +1), sf::Vector3i(-1,  0, +1), sf::Vector3i(-1, +1,  0),
	sf::Vector3i(0, +1, -1), sf::Vector3i(+1,  0, -1), sf::Vector3i(+1, -1,  0)
};

const sf::Vector2i hex_nb_hex[2][6] = {
	{ sf::Vector2i(0, +1), sf::Vector2i(-1, +1), sf::Vector2i(-1, 0),
	sf::Vector2i(0, -1), sf::Vector2i(+1, 0), sf::Vector2i(+1, +1)},
	{ sf::Vector2i(0, +1), sf::Vector2i(-1, 0), sf::Vector2i(-1, -1),
	sf::Vector2i(0, -1), sf::Vector2i(+1, -1), sf::Vector2i(+1, 0) }
};
*/

const sf::Vector2i quad_nb[4] = {
	sf::Vector2i(0, -1), sf::Vector2i(1,  0),
	sf::Vector2i(0, 1), sf::Vector2i(-1, 0)
};

class Terrain : public gameobj {
public:
	int terrainId;
};

enum class Resource { Water = 0, Food = 1, Safety = 2 };

class QuadGrid
{
public:
	QuadGrid();
	~QuadGrid();

	std::vector<Terrain>& getGrid();
	std::vector<float>& getThreatMap();
	void dimensions(const sf::Vector2i& d);

	//mimic vector behaviour
	Terrain &operator[](int i);
	size_t size() const;
	void push_back(const Terrain& o);
	void reserve(size_t s);
	bool insertTerrain(int id, int costs);//return true = insertion | false = assign
	void initThreatMap();
	void clearThreatMap(int val = 0);

	sf::Vector2i dimensions() const;
	int getGridNumber(sf::Vector2f pos) const;
	sf::Vector2i getGridCoords(sf::Vector2f pos) const;
	sf::Vector2i getGridCoords(int pos) const;
	int getTerrain(int key);//TODO: make const
	sf::Vector2f getRealCoords(int p);
	sf::Vector2f getRealCoords(sf::Vector2i p);
	sf::Vector2f getRealCoords(int x, int y);

	//statics
	static int quadDistance(sf::Vector2i a, sf::Vector2i b);
	/*static sf::Vector3i hexToCube(sf::Vector2i hex);
	static sf::Vector2i cubeToHex(sf::Vector3i cube);
	static int hexDistance(sf::Vector2i a, sf::Vector2i b);
	static sf::Vector2i rotate(sf::Vector2i a, sf::Vector2i b, int t);*/

private:
	sf::Vector2i m_dimension;
	std::vector<Terrain> m_grid;
	std::vector<Resource> m_resources;
	std::map<int, int> m_terrains;
};

