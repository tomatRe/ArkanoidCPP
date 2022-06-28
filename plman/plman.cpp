#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <windows.h>

HANDLE consoleHandler;

//Screen info
const int screenX = 120;
const int screenY = 30;


//Character info
COORD playerLocation;
const int playerWidth = 10;
const int playerSpeed = 3;

//Ball info
COORD ballLocation;
int ballDirX = 1;
int ballDirY = -1;

//Blocks info
const int blockWidth = 5;
const int blocksRows = 4;
const int blocksPopulation = 40;
const int blocksSpace = 2;
COORD blocksPositions[blocksPopulation];

//Game info
bool gameOver = false;


int fillFrame() 
{
	for (int i = 0; i < screenY; i++)
	{
		for (int j = 0; j < screenX; j++)
		{
			if (j == screenX-1 || i == screenY-1 || i == 0 || j == 0)
			{
				std::cout << "+";
			}
			else
			{
				std::cout << " ";
			}
		}

		if (i < screenY-1)
			std::cout << "\n";
	}

	return 0;
}


int spawnCharacter()
{
	playerLocation.X = (screenX / 2) - (playerWidth/2);
	playerLocation.Y = 25;

	SetConsoleCursorPosition(consoleHandler, playerLocation);

	for (int i = 0; i < playerWidth; i++)
		std::cout << "#";

	return 0;
}


int spawnBall()
{
	//Ball spawns top-center of the player
	ballLocation.X = playerLocation.X + (playerWidth/2);
	ballLocation.Y = playerLocation.Y - 1;

	SetConsoleCursorPosition(consoleHandler, ballLocation);

	std::cout << "0";

	return 0;
}


int spawnBlocks()
{
	int blocksPerRow = blocksPopulation / blocksRows;

	int rowsWidth = (blockWidth + blocksSpace) * blocksPerRow;
	int coordX = (screenX / 2) - (rowsWidth / 2);

	int rowsHeight = (blocksRows + blocksSpace) / 2;
	int coordY = (screenY / 2) - rowsHeight;

	int i = 0;

	for (int y = coordY; y < rowsHeight + coordY; y++)
	{
		for (int x = coordX; x < blocksPerRow * (blockWidth + blocksSpace) + coordX; x += blockWidth+1)
		{
			blocksPositions[i].X = x;
			blocksPositions[i].Y = y;

			SetConsoleCursorPosition(consoleHandler, blocksPositions[i]);

			for (int j = 0; j < blockWidth; j++)
			{
				std::cout << "@";
			}

			i++;
		}
	}

	return 0;
}


int checkInput()
{
	//Move right
	if (GetKeyState(VK_RIGHT) & 0x8000/*Check if high-order bit is set (1 << 15)*/)
	{
		if (playerLocation.X + playerWidth < screenX - 1)//level boundaries
		{
			playerLocation.X += playerSpeed;
		}
	}

	//Move left
	if (GetKeyState(VK_LEFT) & 0x8000)
	{
		if (playerLocation.X - 1 > 0)//level boundaries
		{
			playerLocation.X -= playerSpeed;
		}
	}

	//Stop game
	if (GetKeyState(VK_ESCAPE) & 0x8000)
	{
		gameOver = true;
	}

	return 0;
}


int movePlayer()
{
	//Delete previous player
	SetConsoleCursorPosition(consoleHandler, playerLocation);
	for (int i = 0; i < playerWidth; i++)
	{
		std::cout << " ";
	}

	checkInput();

	//Draw new position
	SetConsoleCursorPosition(consoleHandler, playerLocation);
	for (int i = 0; i < playerWidth; i++)
	{
		std::cout << "#";
	}

	return 0;
}


int moveBall()
{
	//Delete previous ball
	SetConsoleCursorPosition(consoleHandler, ballLocation);
	std::cout << " ";

	//Calculate screen collision
	if ((ballLocation.X + ballDirX) > (screenX - 2) || ballLocation.X + ballDirX < 1)
	{
		ballDirX = ballDirX * -1;
	}

	if ((ballLocation.Y + ballDirY) > (screenY - 2) || ballLocation.Y + ballDirY < 1)
	{
		ballDirY = ballDirY * -1;
	}

	//Check player collision
	if ((ballLocation.X + ballDirX) >= playerLocation.X &&
		(ballLocation.X + ballDirX) <= playerLocation.X + playerWidth &&
		((ballLocation.Y + ballDirY) == playerLocation.Y ||
		(ballLocation.Y) == playerLocation.Y - 1))
	{
		//Bounce back
		ballDirY = ballDirY * -1;

		//Bounce left/right
		if (ballLocation.X < playerLocation.X+(playerWidth/2))
			ballDirX = -1;

		if (ballLocation.X > playerLocation.X + (playerWidth / 2))
			ballDirX = 1;
	}

	//Check blocks collision
	for (int i = 0; i < blocksPopulation; i++)
	{
		if ((ballLocation.X + ballDirX) >= blocksPositions[i].X &&
			(ballLocation.X + ballDirX) <= blocksPositions[i].X + blockWidth &&
			((ballLocation.Y + ballDirY) == blocksPositions[i].Y ||
			(ballLocation.Y) == blocksPositions[i].Y - 1))
		{
			//Bounce back
			ballDirY = ballDirY * -1;

			//Bounce left/right
			if (ballLocation.X < blocksPositions[i].X + (blockWidth / 2))
				ballDirX = -1;

			if (ballLocation.X > blocksPositions[i].X + (blockWidth / 2))
				ballDirX = 1;

			//Delete block
			SetConsoleCursorPosition(consoleHandler, blocksPositions[i]);

			for (int k = 0; k < blockWidth; k++)
				std::cout << " ";

			blocksPositions[i].X = -1;
			blocksPositions[i].Y = -1;

			//Break for
			i = blocksPopulation;
		}
	}

	//Set new location
	ballLocation.X = ballLocation.X + ballDirX;
	ballLocation.Y = ballLocation.Y + ballDirY;

	//Render new location
	SetConsoleCursorPosition(consoleHandler, ballLocation);
	std::cout << "0";

	return 0;
}


int main()
{
	consoleHandler = GetStdHandle(STD_OUTPUT_HANDLE);

	fillFrame();
	spawnCharacter();
	spawnBall();
	spawnBlocks();

	std::clock_t start;
	double end;

	while (!gameOver)
	{
		start = std::clock();

		movePlayer();
		moveBall();

		end = std::clock() - start;
		int ms = end - 16.f;

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

//std::cout << "Hello World!\n";