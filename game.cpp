#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
using namespace sf;
using namespace std;

//Setting constants as global
int const width = 1280;
int const height = 720;
float const radius = 56.f;
float const WaterSpeed = 150.f;
float const maximumFrameTime = 0.1f;

int const maxNumObstacles = 10;
float const obstacleWidth = 64.f;
float const obstacleHeight = 64.f;
Color const obstacleColor(120, 90, 60);
float const ODL = -500; //Obstacle Distance limit, the maximum distance after the obstacles are despawned

//adding structs to our game
struct WaterMap
{
    float VerticalWaterMovement = 0.f;
    float HorizontalWaterMovement = 0.f;
};

struct Obstacle
{
    float xpos = 0.f;
    float ypos = 0.f;
};

struct GameState
{
    WaterMap waterMap;
    bool scrolling = false;
    int Direction = 0;
	vector <Obstacle> obstacles;
    mt19937 randomizer;
};

//function for the vertical scrolling of water tiles
float verticalMapMovement(float& verticalPosition, float WaterSpeed, float frameTime)
{
    verticalPosition -= WaterSpeed * frameTime;

    if (verticalPosition <= -160.f)
    {
        verticalPosition += 160.f;
    }

    return verticalPosition;
}

//function to calculate the horizontal direction and to set scrolling to of map to false or true depending upon key pressed
int horizontalDirection(Keyboard::Key key, int Direction, bool& scrolling)
{
    if (key == Keyboard::D || key == Keyboard::Right)
    {
        Direction = 1;
        scrolling = true;
    }
    else if (key == Keyboard::A || key == Keyboard::Left)
    {
        Direction = -1;
        scrolling = true;
    }
    else if (key == Keyboard::S || key == Keyboard::Down)
    {
        Direction = 0;
        scrolling = true;
    }
    else if (key == Keyboard::W || key == Keyboard::Up)
    {
        Direction = 0;
        scrolling = false;
    }
    return Direction;
}

//function for the horizontal movement of the water
float horizontalMapMovement(float& HorizontalWaterMovement, int Direction, float WaterSpeed, float frameTime)
{
    HorizontalWaterMovement -= Direction * WaterSpeed * frameTime;

    if (HorizontalWaterMovement <= -160.f)
    {
        HorizontalWaterMovement += 160.f;
    }
    if (HorizontalWaterMovement > 0.f)
    {
        HorizontalWaterMovement -= 160.f;
    }

    return HorizontalWaterMovement;
}

//function for spawning obstacles at random positions on the screen
void ObstacleSpawn(GameState& g)
{
    if (int(g.obstacles.size()) < maxNumObstacles)
    {
        uniform_int_distribution<int> randomX(-width, 2 * width - 1);
        uniform_int_distribution<int> randomY(0, 2 * height - 1);

        Obstacle newObstacle;
        newObstacle.xpos = float(randomX(g.randomizer));
        newObstacle.ypos = float(randomY(g.randomizer));

        if (newObstacle.xpos >= 0.f && newObstacle.xpos <= float(width) && newObstacle.ypos >= 0.f && newObstacle.ypos <= float(height))
        {
            return;
        }

        g.obstacles.push_back(newObstacle);
    }
}

//function for scrolling the obstacles along with the water tiles
void ObstacleScroll(GameState& g, float distanceX, float distanceY)
{
    for (int i = 0; i < int(g.obstacles.size()); ++i)
    {
        g.obstacles[i].xpos += distanceX;
        g.obstacles[i].ypos += distanceY;
    }
}

//function for removing obstacles that have scrolled off the screen
void ObstacleRemoval(GameState& g)
{
    int count = 0;
    while (count < int(g.obstacles.size()))
    {
        if (g.obstacles[count].ypos < float(ODL))
        {
            g.obstacles.erase(g.obstacles.begin() + count);
        }
        else
        {
            ++count;
        }
    }
}


int main() {
    //declaring a variable of gamestate that stores current gamestate (scrolling or stopped)
    GameState gameState;

	//seeding the random number generator for obstacle spawning
    gameState.randomizer.seed(random_device{}());

    //creation of the window
    RenderWindow window(VideoMode(width, height), "Surf", Style::Titlebar | Style::Close);
    window.setVerticalSyncEnabled(true);//used to limit the games frame rate to the pc's refresh rate

    //setting up the circle and all its properties
    CircleShape shape(radius);
    shape.setFillColor(Color::Green);
    shape.setPosition((width / 2.f - radius), (height / 3.f - radius));

    //setting up details of the water map
    Vector2f const TileSize(10.f, 50.f);
    RectangleShape waterTile(TileSize);

    Clock clock;  //declaring a clock to keep counting time between each frame

    while (window.isOpen())
    {
        float frameTime = clock.restart().asSeconds(); //calculating frame time in seconds, using later for horizontal and vertical distance movement
        if (frameTime > maximumFrameTime)
        {
            frameTime = maximumFrameTime;
        }

        //following loop checks if user presses the 'x' or cross button to shut window to close program
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            if (event.type == Event::KeyPressed) // now the horizontal direction movement is checked through event KeyPressed so direction is set once
            {
                gameState.Direction = horizontalDirection(event.key.code, gameState.Direction, gameState.scrolling);
            }
        }

        if (gameState.scrolling) // if map is scrolling only then the horizontal and vertical movement functions are called to move the watertiles
        {
            gameState.waterMap.VerticalWaterMovement = verticalMapMovement(gameState.waterMap.VerticalWaterMovement, WaterSpeed, frameTime);
            gameState.waterMap.HorizontalWaterMovement = horizontalMapMovement(gameState.waterMap.HorizontalWaterMovement, gameState.Direction, WaterSpeed, frameTime);
        }

        window.clear(Color(30, 80, 140));  // screen color set to a blue shade
        //following loop draws water tiles to show details of map
        for (int column = 0; column < 18; column++)
        {
            for (int row = 0; row < 11; row++)
            {
                if ((row + column) % 2 == 0)
                {
                    waterTile.setFillColor(Color(0, 128, 128));
                }
                else
                {
                    waterTile.setFillColor(Color(65, 105, 225));
                }
                waterTile.setPosition((column * 80.f) + gameState.waterMap.HorizontalWaterMovement, (row * 80.f) + gameState.waterMap.VerticalWaterMovement);
                window.draw(waterTile);
            }
        }
        window.draw(shape);
        window.display();
    }

    return 0;
}
