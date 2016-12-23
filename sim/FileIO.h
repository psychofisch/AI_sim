#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>

#include <SFML\System\Vector2.hpp> 

#include "QuadGrid.h"
#include "vector2math.h"

class FileIO
{
public:
	FileIO();
	~FileIO();
	
	static sf::Vector2i LoadLevel(const char* path, QuadGrid& grid, std::vector<sf::Texture*>& textures, bool debug = false);
	static sf::Vector2i LoadLevel(const char* path, const char* texpath, QuadGrid& grid, std::vector<sf::Texture*>& textures, bool debug = false);
	//static bool LoadPath(const char* path, std::vector<sf::Vector2f>& waypoints);

	static std::string LoadText(const char*);
};

