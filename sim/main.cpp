#include <iostream>

#include <SFML\Graphics.hpp>

#include "FileIO.h"
#include "QuadWorld.h"

void main(int argc, const char* argv[])
{
	float tick = .1f;
	std::string pic = "level1.bmp",
				level = "test_level_tex.txt";

	if (argc == 2)
		tick = std::atof(argv[1]);

	if (argc >= 3)
	{
		pic = argv[1];
		level = argv[2];
	}

	if (argc == 4)
	{
		tick = std::atof(argv[3]);
	}

	//init
	sf::VideoMode vm;
	vm.width = 720;
	vm.height = 720;
	vm.bitsPerPixel = 32;	

	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;

	std::cout << "creating window...\n";
	sf::RenderWindow window(vm, "A*", sf::Style::Titlebar | sf::Style::Close | sf::Style::Default, settings);
	window.setVerticalSyncEnabled(false);
	window.setFramerateLimit(60);

	QuadWorld hw;
	hw.tick = tick;
	hw.stepMode = true;
	hw.setRenderWindow(&window);
	hw.loadLevel(pic.c_str(), level.c_str());
	//hw.loadLevel("test_level.txt");

	hw.run();
}
