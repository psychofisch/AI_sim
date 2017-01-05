#include "QuadGrid.h"

#include "Astar.h"

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

std::vector<Resource>& QuadGrid::getResource()
{
	return m_resources;
}

/*std::vector<float>& QuadGrid::getThreatMap()
{
	return m_threat;
}*/

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

bool QuadGrid::isDoor(sf::Vector2i pos)
{
	if (m_grid[getGridNumber(pos)].activity == Terrain::Activity::closedDoor || m_grid[getGridNumber(pos)].activity == Terrain::Activity::openDoor)
		return true;
	else
		return false;
}

void QuadGrid::lock(sf::Vector2i pos, Terrain::Activity a)
{
	if (m_grid[getGridNumber(pos)].activity == Terrain::Activity::closedDoor || m_grid[getGridNumber(pos)].activity == Terrain::Activity::openDoor)
	{
		m_grid[getGridNumber(pos)].activity = a;
	}
}

void QuadGrid::initResources()
{
	m_resources.resize(m_grid.size(), Resource::Empty);
}

/*
void QuadGrid::clearThreatMap(int val)
{
	std::fill(m_threat.begin(), m_threat.end(), val);
}
*/

sf::Vector2i QuadGrid::dimensions() const
{
	return m_dimension;
}

void QuadGrid::dimensions(const sf::Vector2i& d)
{
	m_dimension = d;
	//m_resources.resize(d.x * d.y);
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

inline int QuadGrid::getGridNumber(sf::Vector2i pos) const
{
	return pos.x + pos.y * m_dimension.x;
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

Resource QuadGrid::getResource(int index)
{
	return m_resources[index];
}

Resource QuadGrid::getResource(sf::Vector2i pos)
{
	return getResource(getGridNumber(pos));
}

sf::Vector2f QuadGrid::getRealCoords(int p)
{
	return m_grid[p].getPosition();
}

sf::Vector2f QuadGrid::getRealCoords(sf::Vector2i pos)
{
	return getRealCoords(getGridNumber(pos));
}

sf::Vector2f QuadGrid::getRealCoords(int x, int y)
{
	return getRealCoords(sf::Vector2i(x, y));
}

std::vector<int> QuadGrid::findResource(Resource r)
{
	std::vector<int> result;

	for (int i = 0; i < m_resources.size(); ++i)
	{
		if (m_resources[i] == r)
		{
			/*if (r == Resource::Safety)
			{
				int count = 0;
				for (int n = 0; n < 4; ++n)
				{
					sf::Vector2i tmpPos = getGridCoords(i) + quad_nb[n];
					if (isDoor(tmpPos) && !isLocked(tmpPos))
					{
						count++;
					}
				}
				if(count > 0)
					result.push_back(i);
			}
			else
			{*/
				result.push_back(i);
			//}
		}
	}

	return result;
}

int QuadGrid::findClosestResource(sf::Vector2i pos, Resource r)
{
	std::vector<int> fields = findResource(r);
	int minResInd = -1;
	int minDist = INT_MAX;
	for (int i = 0; i < fields.size(); ++i)
	{
		int tmpDist = Astar::findPath(*this, pos, getGridCoords(fields[i])).size();
		if (tmpDist < minDist)
		{
			minResInd = i;
			minDist = tmpDist;
		}
	}

	if (minResInd != -1)
	{
		return fields[minResInd];
	}
	else
		return -1;
}

bool QuadGrid::isLocked(sf::Vector2i pos)
{
	if (m_grid[getGridNumber(pos)].activity == Terrain::Activity::closedDoor)
		return true;
	else
		return false;
}

int QuadGrid::quadDistance(sf::Vector2i a, sf::Vector2i b)
{
	return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

Terrain& QuadGrid::operator[](int i)
{
	return m_grid[i];
}
