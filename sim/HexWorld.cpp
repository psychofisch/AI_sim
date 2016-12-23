#include "HexWorld.h"



HexWorld::HexWorld()
	:tick(2.f),
	m_step(true),
	stepMode(false)
{
	m_currentTile.setFillColor(sf::Color(255,0,0, 128));
}


HexWorld::~HexWorld()
{
}

void HexWorld::run()
{
	int mouseMoveRectSize = 400;
	sf::IntRect mouseMoveRect = sf::IntRect((m_window->getSize().x - mouseMoveRectSize) / 2, (m_window->getSize().x - mouseMoveRectSize) / 2, mouseMoveRectSize, mouseMoveRectSize);
	sf::Vector2f windowCenter(m_window->getSize());
	windowCenter /= 2.0f;

	sf::Clock time;
	float dt = 0.0f;
	float tickRun = tick;
	int currentTile = -1;

	m_threatTile.setFillColor(sf::Color::Red);

	m_player.setGrid(&m_grid);

	Enemy enemy;
	enemy.setTexture(m_texture, 2);
	int hexOffsetY = 0.07f * m_texture[1]->getSize().y;
	enemy.setTextureRect(sf::IntRect(0, hexOffsetY, m_texture[1]->getSize().x, m_texture[1]->getSize().y - 2 * hexOffsetY));
	enemy.setGrid(&m_grid);
	enemy.isPlayer(false);

	enemy.setPosition(m_grid[3 + 8 * m_grid.dimensions().y].getPosition());
	enemy.addWaypoint(m_grid.getGridCoords(2 + 3 * m_grid.dimensions().y));
	enemy.addWaypoint(m_grid.getGridCoords(11 + 3 * m_grid.dimensions().y));
	m_enemies.push_back(enemy);

	enemy.setPosition(m_grid[24].getPosition());
	enemy.clearWaypoints();
	enemy.addWaypoint(m_grid.getGridCoords(24));
	enemy.addWaypoint(m_grid.getGridCoords(22 + 8 * m_grid.dimensions().y));
	m_enemies.push_back(enemy);

	bool quit = false;
	while (!quit)
	{
		time.restart();

		sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
		sf::Vector2f mousePos_mapped = m_window->mapPixelToCoords(mousePos, m_view);

		//controls
		sf::Event eve;
		while (m_window->pollEvent(eve))
		{
			if (eve.type == sf::Event::Closed)
			{
				quit = true;
				break;
			}
			else if (eve.type == sf::Event::MouseButtonPressed && eve.mouseButton.button == sf::Mouse::Left)
			{
				if (m_player.getTarget() != -1)
					m_grid[m_player.getTarget()].setFillColor(sf::Color::White);

				m_player.setTarget(m_grid.getGridNumber(mousePos_mapped));
				if (m_player.getTarget() == -1)
					break;
				m_grid[m_player.getTarget()].setFillColor(sf::Color::Magenta);

				sf::Vector2i coordsTarget = m_grid.getGridCoords(m_grid[m_player.getTarget()].getPosition());
				sf::Vector2i coordsPlayer = m_grid.getGridCoords(m_player.getPosition());
				std::cout << "Target: " << coordsTarget.x << "|"  << coordsTarget.y 
						<< " - Player: " << coordsPlayer.x << "|" << coordsPlayer.y << " = " << HexGrid::hexDistance(coordsTarget, coordsPlayer) << std::endl;
				//std::cout << mousePos_mapped.x << "," << mousePos_mapped.y << std::endl;
				break;
			}
			else if (eve.type == sf::Event::MouseButtonPressed && eve.mouseButton.button == sf::Mouse::Right)
			{
				int coordsPlayer = m_grid.getGridNumber(mousePos_mapped);
				m_player.setPosition(m_grid[coordsPlayer].getPosition());
				std::cout << mousePos_mapped.x << "," << mousePos_mapped.y << std::endl;
				break;
			}
			else if (eve.type == sf::Event::MouseWheelScrolled)
			{
				if (eve.mouseWheelScroll.delta < 0)
				{
					m_view.zoom(1.1f);
				}
				else if (eve.mouseWheelScroll.delta > 0)
				{
					m_view.zoom(0.9f);
				}
				break;
			}
			else if (eve.type == sf::Event::KeyPressed)
			{
				switch (eve.key.code)
				{
				case sf::Keyboard::H: std::cout << m_player.getRotation() << std::endl;
					break;
				case sf::Keyboard::N:
					m_step = true;
					break;
				case sf::Keyboard::M:
					m_step = true;
					stepMode = !stepMode;
					break;
				case sf::Keyboard::R:
					
					break;
				case sf::Keyboard::Escape:
					quit = true;
					break;
				}
			}
			else if (eve.type == sf::Event::Resized)
			{
				m_view = sf::View(sf::FloatRect(0, 0, eve.size.width, eve.size.height));
				sf::Vector2f view_center((m_grid[0].getPosition() + m_grid[m_grid.size() - 1].getPosition()) / 2.0f);
				m_view.setCenter(view_center);
			}
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Middle) && !mouseMoveRect.contains(mousePos))
		{
			m_view.move(normalize(sf::Vector2f(mousePos) - windowCenter)*50.0f*dt);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			m_view.move(sf::Vector2f(0.f, -1.f)*50.0f*dt);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			m_view.move(sf::Vector2f(-1.f, 0.f)*50.0f*dt);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			m_view.move(sf::Vector2f(0.f, 1.f)*50.0f*dt);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			m_view.move(sf::Vector2f(1.f, 0.f)*50.0f*dt);

		currentTile = m_grid.getGridNumber(mousePos_mapped);
		if (currentTile != -1)
			m_currentTile.setPosition(m_grid[currentTile].getPosition());
		//*** controls

		//updates
		tickRun -= dt;
		if (tickRun <= 0.f)
		{
			if (m_step == true)
			{
				tickRun = tick;
				m_grid.clearThreatMap();
				for (auto&& e : m_enemies)
					e.update(dt);
				m_player.update(dt);
			}

			if (stepMode)
				m_step = false;
		}

		//render
		m_window->clear(sf::Color(69, 69, 69));

		m_window->setView(m_view);

		for (int i = 0; i < m_grid.size(); ++i)
		{
			m_threatTile.setPosition(m_grid[i].getPosition());
			m_threatTile.setFillColor(sf::Color(255, 0, 0, m_grid.getThreatMap()[i] * 255.f));
			m_window->draw(m_grid[i]);

			m_window->draw(m_threatTile/*, sf::RenderStates(sf::BlendMultiply)*/);
		}
		m_player.drawPath(m_window);
		m_window->draw(m_currentTile);

		for (auto&& e : m_enemies)
		{
			m_window->draw(e);
		}

		m_window->draw(m_player);

		m_window->setView(m_window->getDefaultView());

		m_window->display();
		//*** render

		dt = time.getElapsedTime().asSeconds();
	}
}

void HexWorld::loadLevel(const char* path)
{
	FileIO::LoadLevel(path, m_grid, m_texture);

	i_init();
}

void HexWorld::loadLevel(const char * gridpath, const char * texpath)
{
	FileIO::LoadLevel(gridpath, texpath, m_grid, m_texture);

	i_init();
}

void HexWorld::setRenderWindow(sf::RenderWindow * wndw)
{
	m_window = wndw;
}

void HexWorld::i_init()
{
	sf::Vector2f view_center((m_grid[0].getPosition() + m_grid[m_grid.size() - 1].getPosition()) / 2.0f);
	m_view.setCenter(view_center);

	m_player.isPlayer(true);
	m_player.setPosition(m_grid[m_grid.size() / 2].getPosition());
	m_player.setTexture(m_texture, 1);
	int hexOffsetY = 0.07f * m_texture[1]->getSize().y;
	m_player.setTextureRect(sf::IntRect(0, hexOffsetY, m_texture[1]->getSize().x, m_texture[1]->getSize().y - 2 * hexOffsetY));

	m_grid.initThreatMap();
}