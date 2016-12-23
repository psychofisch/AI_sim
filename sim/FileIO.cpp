#include "FileIO.h"

FileIO::FileIO()
{
}


FileIO::~FileIO()
{
}

sf::Vector2i FileIO::LoadLevel(const char* path, HexGrid& grid, std::vector<sf::Texture*>& textures, bool debug)
{
	sf::Vector2i size(0, 0);

	std::ifstream file;
	file.open(path);
	if (!file.is_open())
	{
		std::cout << "Failed to open file: " << path << std::endl;
		return sf::Vector2i(0, 0);
	}

	std::string tmp;
	std::getline(file, tmp);

	//size
	sscanf_s(tmp.c_str(), "%i,%i", &size.x, &size.y);
	if(debug)
		std::cout << "world size = " << size.x << ":" << size.y << std::endl;

	grid.dimensions(size);
	grid.reserve(size.x * size.y);

	//world/background texture
	std::getline(file, tmp);

	{
		sf::Texture* tmp_tex = new sf::Texture();
		tmp_tex->loadFromFile(tmp);
		tmp_tex->setRepeated(true);

		textures.push_back(tmp_tex);
	}

	if(debug)
		std::cout << "world image path = " << tmp << std::endl;

	int row = 0;
	do {
		std::getline(file, tmp);

		if (tmp.size() == 0)
			break;

		if (tmp[0] == 't')
		{
			//std::cout << "Test";
			char tex_path[255];
			sscanf_s(tmp.c_str(), "t,%s", tex_path);

			sf::Texture* tmp_tex = new sf::Texture();
			tmp_tex->loadFromFile(tex_path);
			tmp_tex->setSmooth(true);
			textures.push_back(tmp_tex);

			std::cout << "image " << textures.size() - 1 << " path = " << tex_path << std::endl;
		}
		else
		{
			Terrain tmp_obj;
			//tmp_obj.setOutlineThickness(.2f);
			//tmp_obj.setOutlineColor(sf::Color::Red);
			for (int i = 0; i < tmp.size(); ++i)
			{
				if (tmp[i] == ',')
				{
					if (debug)
						std::cout << tmp[i] - '0';
					continue;
				}

				tmp_obj.setTexture(textures, tmp[i] - '0');
				sf::Vector2f size_fac = tmp_obj.getSizeFactors();

				if(grid.size() % 2 == 0)
					tmp_obj.setPosition(sf::Vector2f(i * tmp_obj.getRadius() * 0.75, row * size_fac.y));
				else
					tmp_obj.setPosition(sf::Vector2f(i * tmp_obj.getRadius() * 0.75f, row * size_fac.y - (0.866f * tmp_obj.getRadius())));

				grid.push_back(tmp_obj);
			}

			row++;
			if (debug)
				std::cout << std::endl;
			//tmp_obj.setRotation(rot);
			//tmp_obj.shape.setOrigin(textures[tmp_obj.textureId + 1]->getSize().x / 2, textures[tmp_obj.textureId + 1]->getSize().y / 2);
		}
		
	} while (!file.eof());

	file.close();

	return size;
}
sf::Vector2i FileIO::LoadLevel(const char * gridpath, const char* texpath, HexGrid& grid, std::vector<sf::Texture*>& textures, bool debug)
{
	//load Textures
	std::ifstream file;
	file.open(texpath);
	if (!file.is_open())
	{
		std::cout << "Failed to open file: " << texpath << std::endl;
		return sf::Vector2i(0, 0);
	}

	std::string tmp;

	//world/background texture
	std::getline(file, tmp);
	if (tmp[0] != 'o')
	{
		std::cout << "levelfile corrupt: " << texpath << std::endl;
		return sf::Vector2i(0, 0);
	}

	int texOffset = 0;
	std::getline(file, tmp);

	do{
		sf::Texture* tmp_tex = new sf::Texture();
		tmp_tex->loadFromFile(tmp);
		tmp_tex->setRepeated(true);

		textures.push_back(tmp_tex);
		texOffset++;
		std::getline(file, tmp);
	} while (tmp[0] != 't');

	int row = 1;
	do {
		std::string terrain;
		std::getline(file, tmp, ',');
		std::getline(file, terrain);

		if (tmp.size() == 0)
			break;

		sf::Texture* tmp_tex = new sf::Texture();
		tmp_tex->loadFromFile(tmp);
		tmp_tex->setSmooth(true);
		textures.push_back(tmp_tex);
		
		grid.insertTerrain(textures.size() - 1, atoi(terrain.c_str()));

		std::cout << "image " << textures.size() - 1 << " path = " << tmp << std::endl;
	} while (!file.eof());
	file.close();
	//***

	//fill grid
	sf::Vector2i size;
	sf::Image lvl;
	lvl.loadFromFile(gridpath);
	size.x = lvl.getSize().x;
	size.y = lvl.getSize().y;
	grid.dimensions(size);
	grid.reserve(size.x * size.y);
	Terrain tmp_obj;
	//tmp_obj.setOutlineThickness(-.2f);
	tmp_obj.setOutlineColor(sf::Color::Red);
	for(int y = 0; y < size.y; ++y)
		for (int x = 0; x < size.x; ++x)
		{
			int texId = lvl.getPixel(x, y).r/10;
			texId += 2;
			if (textures.size() > texId)
			{
				tmp_obj.setFillColor(sf::Color::White);
				tmp_obj.setTexture(textures, texId);

				int hexOffsetY = 0.07f * textures[texId]->getSize().y;
				tmp_obj.setTextureRect(sf::IntRect(0, hexOffsetY, textures[texId]->getSize().x, textures[texId]->getSize().y - 2 * hexOffsetY));
			}
			else
			{
				tmp_obj.setFillColor(sf::Color::Magenta);
				tmp_obj.setTexture(textures, -1);
			}
			sf::Vector2f size_fac = tmp_obj.getSizeFactors();

			if (grid.size() % 2 == 0)
				tmp_obj.setPosition(sf::Vector2f(x * tmp_obj.getRadius() * 1.5f, y * size_fac.y));
			else
				tmp_obj.setPosition(sf::Vector2f(x * tmp_obj.getRadius() * 1.5f, y * size_fac.y - (0.866f * tmp_obj.getRadius())));

			grid.push_back(tmp_obj);
		}
	//***

	return size;
}
/*
bool FileIO::LoadPath(const char * path, std::vector<sf::Vector2f>& waypoints)
{
	std::ifstream file;
	file.open(path);
	if (!file.is_open())
	{
		std::cout << "Failed to open file: " << path << std::endl;
		return false;
	}

	do {
		std::string tmp;
		std::getline(file, tmp);

		if (tmp.size() == 0)
			break;

		float x, y;
		sscanf_s(tmp.c_str(), "%f,%f", &x, &y);

		waypoints.push_back(sf::Vector2f(x, y));
	} while (!file.eof());

	file.close();

	return true;
}
*/
std::string FileIO::LoadText(const char* path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "Failed to open file: " << path << std::endl;
		return "Error";
	}

	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	
	file.close();

	return content;
}
