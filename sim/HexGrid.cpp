#include "HexGrid.h"



HexGrid::HexGrid()
{
}


HexGrid::~HexGrid()
{
}

std::vector<Terrain>& HexGrid::getGrid()
{
	return m_grid;
}

std::vector<float>& HexGrid::getThreatMap()
{
	return m_threat;
}

size_t HexGrid::size() const
{
	return m_grid.size();
}

void HexGrid::push_back(const Terrain & o)
{
	m_grid.push_back(o);
}

void HexGrid::reserve(size_t s)
{
	m_grid.reserve(s);
}

bool HexGrid::insertTerrain(int id, int costs)
{
	return m_terrains.insert_or_assign(id, costs).second;
}

void HexGrid::initThreatMap()
{
	m_threat.resize(m_grid.size(), .0f);
}

void HexGrid::clearThreatMap(int val)
{
	std::fill(m_threat.begin(), m_threat.end(), val);
}

sf::Vector2i HexGrid::dimensions() const
{
	return m_dimension;
}

void HexGrid::dimensions(const sf::Vector2i& d)
{
	m_dimension = d;
	m_threat.resize(d.x * d.y);
}

int HexGrid::getGridNumber(sf::Vector2f pos) const
{
	int result = -1;
	float dist = INFINITY;
	for (int i = 0; i < m_grid.size(); ++i)
	{
		float tmpdist = magnitude(pos - m_grid[i].getPosition());
		if (tmpdist < dist)
		{
			result = i;
			dist = tmpdist;
		}
	}

	return result;
}

sf::Vector2i HexGrid::getGridCoords(sf::Vector2f pos) const
{
	return getGridCoords(getGridNumber(pos));
}

sf::Vector2i HexGrid::getGridCoords(int pos) const
{
	sf::Vector2i coords;
	coords.x = pos % m_dimension.y;
	coords.y = pos / m_dimension.x;
	return coords;
}

int HexGrid::getTerrain(int key)
{
	//return m_terrains.at(key);
	return m_terrains[key];//operator[] const?
}

Terrain& HexGrid::operator[](int i)
{
	return m_grid[i];
}

sf::Vector3i HexGrid::hexToCube(sf::Vector2i hex)
{
	sf::Vector3i cube;
	cube.x = hex.x;
	cube.z = hex.y - (hex.x + (hex.x & 1)) / 2;
	cube.y = -cube.x - cube.z;
	return cube;
}

sf::Vector2i HexGrid::cubeToHex(sf::Vector3i cube)
{
	sf::Vector2i hex;
	hex.x = cube.x;
	hex.y = cube.z + (cube.x + (cube.x & 1)) / 2;
	return hex;
}

int HexGrid::hexDistance(sf::Vector2i a, sf::Vector2i b)
{
	sf::Vector3i cubeA = hexToCube(a);
	sf::Vector3i cubeB = hexToCube(b);
	return (fabsf(cubeA.x - cubeB.x) + fabsf(cubeA.y - cubeB.y) + fabsf(cubeA.z - cubeB.z)) / 2.f;
}

sf::Vector2i HexGrid::rotate(sf::Vector2i a, sf::Vector2i b, int t)
{
	sf::Vector3i cube = hexToCube(b - a);
	for (int i = 0; i < t; ++i)
	{
		cube.x = -cube.z;
		cube.y = -cube.x;
		cube.z = -cube.y;
	}
	sf::Vector2i c = cubeToHex(cube);
	return c;
}
