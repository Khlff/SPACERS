////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>

////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////

void PhysicMoving(sf::RectangleShape* Ship,float boostDown, float v_up, float dt, float* vy);
void PhysicKick(float v_up, float* vy, bool direction);

int main()
{
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	// Define some constants
	const float pi = 3.14159f;
	const int gameWidth = 1100;
	const int gameHeight = 1200;
	sf::Vector2f shipSize(70, 70);

	// Create the window of the application
	sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "SFML SPACERS",
		sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);

	// Load the sounds used in the game
	sf::SoundBuffer shipSoundBuffer;
	if (!shipSoundBuffer.loadFromFile("resources/ball.wav"))
		return EXIT_FAILURE;
	sf::Sound shipSound(shipSoundBuffer);

	// Load the text font
	sf::Font font;
	if (!font.loadFromFile("resources/sansation.ttf"))
		return EXIT_FAILURE;

	// Create the Ship
	sf::RectangleShape Ship;
	Ship.setSize(shipSize - sf::Vector2f(3, 3));
	Ship.setOutlineThickness(3);
	Ship.setOutlineColor(sf::Color::Black);
	Ship.setFillColor(sf::Color::White);
	Ship.setOrigin(shipSize / 2.f);

	float vy_ship = 0;

	float boostFall = 0.002;
	float velocityUp = 0.55;

	// Create the verticalEdges
	sf::CircleShape spikeDown(65.f, 3);
	int counterOfDownSpikes = 0;
	float xSpikesPos = 0;
	//sf::CircleShape ball;
	//ball.setRadius(ballRadius - 3);
	//ball.setOutlineThickness(3);
	//ball.setOutlineColor(sf::Color::Black);
	//ball.setFillColor(sf::Color::White);
	//ball.setOrigin(ballRadius / 2, ballRadius / 2);

	// Initialize the pause message
	sf::Text pauseMessage;
	pauseMessage.setFont(font);
	pauseMessage.setCharacterSize(40);
	pauseMessage.setPosition(170.f, 150.f);
	pauseMessage.setFillColor(sf::Color::White);
	pauseMessage.setString("Welcome to SPACERS!\nPress space to start the game");

	// Define the ships properties
	sf::Clock AITimer;
	const sf::Time AITime = sf::seconds(0.1f);
	const float ShipSpeed = 400.f;

	bool direction = true;
	float gameSpeed = 10.0f;

	sf::Clock clock;
	bool isPlaying = false;

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

					// Reset the position of the Ship
					Ship.setPosition(gameWidth / 2, gameHeight / 2);
					direction = true;
					vy_ship = 0;
					boostFall = 0.002;
					velocityUp = 0.55;
					
				}
			}
		}

		if (isPlaying)
		{
			// Check collisions between the ship and the screen (up/down)
			if (Ship.getPosition().y > gameHeight || Ship.getPosition().y < 0)
			{
				isPlaying = false;
				pauseMessage.setString("You lost!\nPress space to restart or\nescape to exit");
			}

			// Check collisions between the ship and the screen (right/left)
			if (Ship.getPosition().x+Ship.getSize().x/2 > gameWidth) {
				direction = false;
				shipSound.play();
			}
			if (Ship.getPosition().x - Ship.getSize().x / 2 < 0) {
				direction = true;
				shipSound.play();
			}

			PhysicMoving(&Ship, boostFall, velocityUp, deltaTime, &vy_ship);
			
			if (counterOfDownSpikes < 10) {
				spikeDown.setPosition(xSpikesPos, gameHeight - 100);
				xSpikesPos += 65.0f;
			}
			else {
				xSpikesPos = 0;
				counterOfDownSpikes = 0;
			}
			// Move the player's ship
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
				vy_ship = 0;
				vy_ship -= velocityUp;
			}

			if (direction) Ship.move(gameSpeed, 0.0f);
			else Ship.move(-gameSpeed, 0.0f);
		}

		// Clear the window
		window.clear(sf::Color(13, 25, 92));

		if (isPlaying)
		{
			//window.draw(verticalEdges);
			window.draw(Ship);
			window.draw(spikeDown);
		}
		else
		{
			// Draw the pause message
			window.draw(pauseMessage);
		}

		// Display things on screen
		window.display();
	}

	return EXIT_SUCCESS;
}

// Function of falling
void PhysicMoving(sf::RectangleShape* Ship, float boostDown, float v_up, float dt, float* vy)
{
	float y = Ship->getPosition().y;
	(*vy) += boostDown * dt;
	Ship->move(0, (*vy) * dt);
	std::cout << Ship->getPosition().x << ' '<< Ship->getPosition().y << std::endl;
};

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
