////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include "vector"
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

// Function prototypes
void PhysicMoving(sf::RectangleShape* Ship, sf::Sprite* bubbles, float boostDown, float v_up, float dt, float* vy);
std::vector<std::pair<float, float>> generateSpikesCoordinates(bool direction);
std::pair<float, float> generatePlanetCoordinates();


int main()
{
	std::srand(static_cast<unsigned int>(std::time(0)));

	// Define some constants
	const int gameWidth = 1100;
	const int gameHeight = 1200;
	sf::Vector2f shipSize(170, 170);

	// Input gamespeed from gamespeed.txt
	std::fstream fileOfSpeed("gamespeed.txt", std::ios::in | std::ios::out | std::ios::app);
	float gameSpeed;
	fileOfSpeed >> gameSpeed;

	// Create the window of the application
	sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "SFML SPACERS",
		sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);


	//	  ----LOADING FILES----

	// Load the sounds used in the game
	sf::SoundBuffer shipSoundBuffer;
	if (!shipSoundBuffer.loadFromFile("resources/bounce.wav"))
		return EXIT_FAILURE;
	sf::Sound shipSound(shipSoundBuffer);

	sf::SoundBuffer shipExplosion;
	shipExplosion.loadFromFile("resources/explosion.wav");
	sf::Sound shipExplosionSound(shipExplosion);

	sf::SoundBuffer planetCollect;
	planetCollect.loadFromFile("resources/planetCollect.wav");
	sf::Sound planetCollectSound(planetCollect);

	sf::Music music;
	music.openFromFile("resources/music.ogg");
	music.setLoop(true);

	// Load the text font
	sf::Font font;
	if (!font.loadFromFile("resources/sansation.ttf"))
		return EXIT_FAILURE;


	//    ----LOADING TEXTURES----

	sf::Texture texture_ship;
	texture_ship.loadFromFile("resources/ship.png");

	sf::Texture texture_space;
	texture_space.loadFromFile("resources/space.jpg");

	sf::Texture texture_bubbles;
	texture_bubbles.loadFromFile("resources/bubbles.png");

	sf::Texture texture_gray;
	texture_gray.loadFromFile("resources/gray.jpg");

	sf::Texture sad_pepe_texture;
	sad_pepe_texture.loadFromFile("resources/sad_pepe.png");

	sf::Texture planets_texture;
	planets_texture.loadFromFile("resources/planets.png");


	//    ----CREATE OBJECTS----
	
	// Create space
	sf::Sprite space(texture_space);
	space.setOrigin(0, 2400);
	space.setPosition(0, 0);

	// Create sad pepe
	sf::Sprite sad_pepe(sad_pepe_texture);
	sad_pepe.setOrigin(495 / 2, 252);
	sad_pepe.setPosition(gameWidth / 2-303, gameHeight / 2+150);
	
	// Create bubbles
	sf::Sprite bubbles(texture_bubbles);
	bubbles.setOrigin(571 / 6.f, 113);
	bubbles.setScale(0.5, 0.5);

	// Load HighScore
	std::fstream fileOfHighscore("highscore.txt", std::ios::in | std::ios::out | std::ios::app);
	int highscore;
	fileOfHighscore >> highscore;

	// Create the Ship
	sf::RectangleShape Ship;
	Ship.setTexture(&texture_ship);
	Ship.setSize(shipSize - sf::Vector2f(3, 3));
	Ship.setOrigin(shipSize / 2.f);

	// Create the edges
	sf::Vector2f edgesSize(50, gameHeight);
	sf::RectangleShape edges;
	edges.setSize(edgesSize);
	edges.setTexture(&texture_gray);
	edges.setOrigin(edgesSize / 2.f);
	edges.setOutlineColor(sf::Color::Black);
	edges.setOutlineThickness(4.0f);

	// Create the spike
	sf::CircleShape spike(60.f, 3);
	spike.setTexture(&texture_gray);
	spike.setRotation(30.f);
	spike.setOutlineColor(sf::Color::Black);
	spike.setOutlineThickness(4.0f);

	// Create the pause message
	sf::Text pauseMessage;
	pauseMessage.setFont(font);
	pauseMessage.setCharacterSize(40);
	pauseMessage.setPosition(170.f, 150.f);
	pauseMessage.setFillColor(sf::Color::White);
	pauseMessage.setString("Welcome to SPACERS!\nPress space to start the game\n\n\nHighscore: " +std::to_string(highscore) + "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n                       Programmed by @Khlff using SFML =)");

	// Create the score
	sf::Text scoreMessage;
	scoreMessage.setFont(font);
	scoreMessage.setCharacterSize(75);
	scoreMessage.setPosition(gameWidth/2.f - 3.f, 100.f);
	scoreMessage.setFillColor(sf::Color::White);

	// Create the planet
	sf::Sprite planet(planets_texture);
	planet.setOrigin(156.5, 196);
	planet.setScale(0.5, 0.5);

	// Define the ship`s properties
	bool direction = true;
	int score = 0;
	std::vector<std::pair<float, float>> coordinatesOfSpikes;
	std::pair<float, float> coordinatesOfPlanet;
	int numberOfPlanetTexture;
	bool makePlanetFlag = false;

	float vy_ship = 0;
	float boostFall = 0.002f;
	float velocityUp = 0.55f;

	sf::Clock clock;
	bool isPlaying = false;
	float CurrentFrame = 0;

	music.play();

	while (window.isOpen())
	{
		float deltaTime = clock.getElapsedTime().asMicroseconds();
		deltaTime /= 1000;
		clock.restart();

		// Handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Window closed or escape key pressed: exit
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
			{
				// Save new highscore
				std::fstream fileOfHighscore("highscore.txt", std::ios::in | std::ios::out);
				fileOfHighscore << highscore;
				window.close();
				break;
			}

			// Space key pressed: play
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space))
			{
				if (!isPlaying)
				{
					// (re)start the game
					isPlaying = true;
					clock.restart();

					// Reset the values
					Ship.setPosition(gameWidth / 2, gameHeight / 2);
					bubbles.setPosition(gameWidth / 2, gameHeight / 2 + 70);
					coordinatesOfSpikes = generateSpikesCoordinates(direction);
					coordinatesOfPlanet = generatePlanetCoordinates();
					planet.setPosition(coordinatesOfPlanet.first, coordinatesOfPlanet.second);
					numberOfPlanetTexture = rand() % 8;
					window.draw(scoreMessage);
					vy_ship = 0;
					boostFall = 0.002f;
					velocityUp = 0.55f;
				}
			}
		}

		if (isPlaying)
		{
			// Check collisions between the ship and the screen (up/down)
			if (Ship.getPosition().y > gameHeight || Ship.getPosition().y < 0)
			{
				shipExplosionSound.play();
				if (score > highscore) highscore = score;
				pauseMessage.setString("You lost!\nPress space to restart or\nescape to exit\n\n\n\n\n\n\n\n\n\n\n                                                Your score is:" + std::to_string(score) + "\n\n                                               Highscore is:" + std::to_string(highscore));
				isPlaying = false;
				score = 0;
				scoreMessage.setString(std::to_string(score));
				gameSpeed = 10.f;
			}

			// Check collisions between the ship and the screen (right/left)
			if (Ship.getPosition().x + Ship.getSize().x / 2 > gameWidth) {
				direction = false;
				shipSound.play();
				score++;
				gameSpeed += 0.25;
				scoreMessage.setString(std::to_string(score));
				coordinatesOfSpikes = generateSpikesCoordinates(direction);
				spike.scale(1, -1);
				if (makePlanetFlag) {
					coordinatesOfPlanet = generatePlanetCoordinates();
					planet.setPosition(coordinatesOfPlanet.first, coordinatesOfPlanet.second);
					numberOfPlanetTexture = rand() % 8;
					makePlanetFlag = false;
				}
			}
			if (Ship.getPosition().x - Ship.getSize().x / 2 < 0) {
				direction = true;
				shipSound.play();
				score++;
				gameSpeed += 0.25;
				scoreMessage.setString(std::to_string(score));
				coordinatesOfSpikes = generateSpikesCoordinates(direction);
				spike.scale(1, -1);
			}

			// Check collisions between the ship and the planet
			if (Ship.getGlobalBounds().intersects(planet.getGlobalBounds())) {
				makePlanetFlag = true;
				planet.setPosition(-100, -100);
				score++;
				scoreMessage.setString(std::to_string(score));
				planetCollectSound.play();
			}

			// Move the player's ship and bubbles
			PhysicMoving(&Ship, &bubbles, boostFall, velocityUp, deltaTime, &vy_ship);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
				vy_ship = 0;
				vy_ship -= velocityUp*1.1;
			}

			if (direction) {
				Ship.move(gameSpeed * deltaTime * 0.07, 0.0f);
				bubbles.move(gameSpeed * deltaTime * 0.07, 0.0f);
			}
			else {
				Ship.move(-gameSpeed * deltaTime * 0.07, 0.0f);
				bubbles.move(-gameSpeed * deltaTime * 0.07, 0.0f);
			}
		}

		window.draw(space);

		// ----DRAWING----
		if (isPlaying)
		{	
			// Draw bubbles
			CurrentFrame += 0.004 * deltaTime;
			if (CurrentFrame > 3) CurrentFrame -= 3;
			bubbles.setTextureRect(sf::IntRect(571 / 3 * int(CurrentFrame), 0, 571 / 3, 226));

			// Draw score message
			window.draw(scoreMessage);
			edges.setPosition(0,gameHeight/2);
			window.draw(edges);
			edges.setPosition(gameWidth,gameHeight / 2);
			window.draw(edges);

			// Draw falling space
			space.move(0, 4);
			if (space.getPosition() == sf::Vector2f(0, 2400)) space.setPosition(0, 0);

			// Draw spikes
			for (int i = 0; i < coordinatesOfSpikes.size(); i++) {
				spike.setPosition(coordinatesOfSpikes[i].first, coordinatesOfSpikes[i].second);
				window.draw(spike);

				// Check collisions between spikes and ship
				if (direction) {
					if (Ship.getGlobalBounds().contains(spike.getPosition() + sf::Vector2f(0, 60))) {
						isPlaying = false;
						shipExplosionSound.play();
						if (score > highscore) highscore = score;
						pauseMessage.setString("You lost!\nPress space to restart or\nescape to exit\n\n\n\n\n\n\n\n\n\n\n                                                Your score is:" + std::to_string(score) + "\n\n                                               Highscore is:" + std::to_string(highscore));
						score = 0;
						scoreMessage.setString(std::to_string(score));
						gameSpeed = 10.f;
						direction = true;
						break;
					}
				}
				else {
					if (Ship.getGlobalBounds().contains(spike.getPosition() + sf::Vector2f(+55, -30))) {
						isPlaying = false;
						shipExplosionSound.play();
						if (score > highscore) highscore = score;
						pauseMessage.setString("You lost!\nPress space to restart or\nescape to exit\n\n\n\n\n\n\n\n\n\n\n                                                Your score is:" + std::to_string(score) + "\n\n                                               Highscore is:" + std::to_string(highscore));
						score = 0;
						scoreMessage.setString(std::to_string(score));
						gameSpeed = 10.f;
						direction = false;
						break;
					}
				}
			}

			// Draw planet
			planet.setTextureRect(sf::IntRect(156.5 * numberOfPlanetTexture, 0, 156.5, 196));
			window.draw(planet);

			window.draw(Ship);
			window.draw(bubbles);
		}
		else {
			window.draw(pauseMessage);
			window.draw(sad_pepe);
		}
		// Display things on screen
		window.display();
	}
	return EXIT_SUCCESS;
}

// Function of falling
void PhysicMoving(sf::RectangleShape* Ship, sf::Sprite* bubbles, float boostDown, float v_up, float dt, float* vy)
{
	float y = Ship->getPosition().y;
	(*vy) += boostDown * dt;
	Ship->move(0, (*vy) * dt);
	bubbles->move(0, (*vy) * dt);
};


// Function for generate spikes coordinates 
std::vector<std::pair<float, float>> generateSpikesCoordinates(bool direction) {
	int countSpikes = 0;
	while (countSpikes == 0 || countSpikes == 1 || countSpikes == 2) countSpikes = rand() % 6;
	// 8 spike max in screen
	std::vector<std::pair<float, float>> coordinates;
	if (direction) coordinates.push_back(std::make_pair(1100.f - 81, rand() % 1200));
	else coordinates.push_back(std::make_pair(-23.f, rand() % 1200));
	for (int i = 0; i < countSpikes-1; i++) {
		if (direction) {
			bool bFlag = true;
			int y;
			int count = 0;
			while (bFlag) {
				y = rand() % 1200;
				for (int j = 0; j < coordinates.size(); j++) if (abs(coordinates[j].second - y) < 100) count++;
				if (count == 0) bFlag = false;
				else count = 0;
			}
			coordinates.push_back(std::make_pair(1100.f - 81, y));
		}
		else {
			bool bFlag = true;
			int y;
			int count = 0;
			while (bFlag) {
				y = rand() % 1200;
				for (int j = 0; j < coordinates.size(); j++) if (abs(coordinates[j].second - y) < 100) count++;
				if (count == 0) bFlag = false;
				else count = 0;
			}
			coordinates.push_back(std::make_pair(0 - 23.f, y));
		}
	}
	return coordinates;
}


// Function for generate planets
std::pair<float, float> generatePlanetCoordinates() {
	float x = rand() % 700;
	while(x<200) x = rand() % 700;
	float y = rand() % 700;
	while (y < 200) y = rand() % 700;
	return std::make_pair(x, y);
}