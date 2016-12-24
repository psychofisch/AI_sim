#include "QuadGrid.h"



QuadGrid::QuadGrid()
{
}


QuadGrid::~QuadGrid()
{
}

std::vector<Terrain>& QuadGrid::getGrid()
{
	return m_grid;
}

std::vector<float>& QuadGrid::getThreatMap()
{
	return m_threat;
}

size_t QuadGrid::size() const
{
	return m_grid.size();
}

void QuadGrid::push_back(const Terrain & o)
{
	m_grid.push_back(o);
}

void QuadGrid::reserve(size_t s)
{
	m_grid.reserve(s);
}

bool QuadGrid::insertTerrain(int id, int costs)
{
	return m_terrains.insert_or_assign(id, costs).second;
}

void QuadGrid::initThreatMap()
{
	m_threat.resize(m_grid.size(), .0f);
}

void QuadGrid::clearThreatMap(int val)
{
	std::fill(m_threat.begin(), m_threat.end(), val);
}

sf::Vector2i QuadGrid::dimensions() const
{
	return m_dimension;
}

void QuadGrid::dimensions(const sf::Vector2i& d)
{
	m_dimension = d;
	m_threat.resize(d.x * d.y);
}

int QuadGrid::getGridNumber(sf::Vector2f pos) const
{
	int result = -1;
	float dist = INFINITY;
	for (int i = 0; i < m_grid.size(); ++i)
	{
		if (m_grid[i].getGlobalBounds().contains(pos))
		{
			result = i;
			break;
		}
	}

	return result;
}

sf::Vector2i QuadGrid::getGridCoords(sf::Vector2f pos) const
{
	return getGridCoords(getGridNumber(pos));
}

sf::Vector2i QuadGrid::getGridCoords(int pos) const
{
	sf::Vector2i coords;
	coords.x = pos % m_dimension.y;
	coords.y = pos / m_dimension.x;
	return coords;
}

int QuadGrid::getTerrain(int key)
{
	//return m_terrains.at(key);
	return m_terrains[key];//operator[] const?
}

sf::Vector2f QuadGrid::getRealCoords(int p)
{
	return m_grid[p].getPosition();
}

sf::Vector2f QuadGrid::getRealCoords(sf::Vector2i p)
{
	return getRealCoords(p.x + p.y * m_dimension.x);
}

sf::Vector2f QuadGrid::getRealCoords(int x, int y)
{
	return getRealCoords(sf::Vector2i(x, y));
}

int QuadGrid::quadDistance(sf::Vector2i a, sf::Vector2i b)
{
	return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

Terrain& QuadGrid::operator[](int i)
{
	return m_grid[i];
}

/*
sf::Vector3i QuadGrid::hexToCube(sf::Vector2i hex)
{
	sf::Vector3i cube;
	cube.x = hex.x;
	cube.z = hex.y - (hex.x + (hex.x & 1)) / 2;
	cube.y = -cube.x - cube.z;
	return cube;
}

sf::Vector2i QuadGrid::cubeToHex(sf::Vector3i cube)
{
	sf::Vector2i hex;
	hex.x = cube.x;
	hex.y = cube.z + (cube.x + (cube.x & 1)) / 2;
	return hex;
}

int QuadGrid::hexDistance(sf::Vector2i a, sf::Vector2i b)
{
	sf::Vector3i cubeA = hexToCube(a);
	sf::Vector3i cubeB = hexToCube(b);
	return (fabsf(cubeA.x - cubeB.x) + fabsf(cubeA.y - cubeB.y) + fabsf(cubeA.z - cubeB.z)) / 2.f;
}


sf::Vector2i QuadGrid::rotate(sf::Vector2i a, sf::Vector2i b, int t)
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
*/