#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <string>
#include <fstream>
#include <cmath>
using namespace sf;
using namespace std;

//Setting constants as global
int const width = 1280;
int const height = 720;
float const radius = 56.f;
float const WaterSpeed = 150.f;
float const maximumFrameTime = 0.1f;

int const maxNumObstacles = 20;
float const obstacleWidth = 64.f;
float const obstacleHeight = 64.f;
Color const obstacleColor(120, 90, 60);
float const ODL = -500; //Obstacle Distance limit, the maximum distance after the obstacles are despawned

float const boosterWidth = 48.f;
float const boosterHeight = 48.f;
Color const boosterColor(255, 220, 0);
int const numBoosterSpots = 4;
Vector2f const boosterSpots[numBoosterSpots] = {
    Vector2f(160.f, height + 40.f),
    Vector2f(480.f, height + 40.f),
    Vector2f(800.f, height + 40.f),
    Vector2f(1120.f, height + 40.f)
};
int const boosterSpawnChance = 300;

float const boostSpeedMultiplier = 3.f;
float const obstacleSpeedReducer = 0.3f;
float const boostDuration = 2.f;
float const obstacleCollisionCoolDown = 2.f; // value raised from 0.3f, it now acts as the invincibility duration
float const slowDuration = 0.5f;             // how long you are slowed after losing a life
float const speedUpDistance = 500.f;         // in meters (the same value shown on screen)
float const speedUpMultiplier = 1.2f;        // speed multiplier (water and monsters) once speedUpDistance is reached

//constants for lives
float const lifeWidth = 40.f;
float const lifeHeight = 40.f;
Color const lifeColor(220, 20, 60);
int const numLifeSpots = 3;
Vector2f const lifeSpots[numLifeSpots] = {
    Vector2f(320.f, height + 40.f),
    Vector2f(640.f, height + 40.f),
    Vector2f(960.f, height + 40.f)
};
int const lifeSpawnChance = boosterSpawnChance * 4;
int const maxLives = 3;
int const startingLives = 3;

//constants for the monster
float const monsterWidth = 50.f;
float const monsterHeight = 50.f;
Color const monsterColor(150, 0, 200);
int const monsterSpawnChance = lifeSpawnChance;
float const monsterSpeedMultiplier = 1.2f;
float const monsterODL = -2000.f; // monsters are removed much later than obstacles
int const numMonsterSpots = 3;
Vector2f const monsterSpots[numMonsterSpots] = {
    Vector2f(240.f, height + 40.f),
    Vector2f(560.f, height + 40.f),
    Vector2f(880.f, height + 40.f)
};
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
    bool isBooster = false; // true if this obstacle is a booster triangle
    bool isLife = false;    // true if this obstacle is a life pickup (if both are false it is a normal obstacle)
};

struct Monster
{
    float xpos = 0.f;
    float ypos = 0.f;
    bool isChasing = false;
};
struct GameState
{
    WaterMap waterMap;
    bool scrolling = false;
    int Direction = 0;
    vector <Obstacle> obstacles;
    vector<Monster> monsters;
    mt19937 randomizer;
    float boostTimer = 0.f;
    float obstacleCollisionTimer = 0.f;
    float slowTimer = 0.f; // > 0 means the water speed is reduced
    int lives = startingLives;
    float distance = 0.f; // total distance travelled, used as the score
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

//returns the bounds of an obstacle/booster/life depending on what it is
FloatRect ObstacleBounds(Obstacle const& o)
{
    if (o.isBooster)
    {
        return FloatRect(o.xpos, o.ypos, boosterWidth, boosterHeight);
    }
    if (o.isLife)
    {
        return FloatRect(o.xpos, o.ypos, lifeWidth, lifeHeight);
    }
    return FloatRect(o.xpos, o.ypos, obstacleWidth, obstacleHeight);
}

//function for a bug we saw, this removed any overlapping boosts, lives or obstacles
bool SpotOverlapsObstacle(GameState const& g, FloatRect spot)
{
    for (int i = 0; i < int(g.obstacles.size()); ++i)
    {
        if (spot.intersects(ObstacleBounds(g.obstacles[i])))
        {
            return true;
        }
    }
    return false;
}

// used for collision with boost, life or obstacle
void ObstacleCollision(GameState& g, FloatRect playerBounds)
{
    int count = 0;
    while (count < int(g.obstacles.size()))
    {
        if (playerBounds.intersects(ObstacleBounds(g.obstacles[count])))
        {
            if (g.obstacles[count].isBooster)
            {
                g.obstacles.erase(g.obstacles.begin() + count);
                g.boostTimer = boostDuration;
            }
            else if (g.obstacles[count].isLife)
            {
                g.obstacles.erase(g.obstacles.begin() + count);
                if (g.lives < maxLives)
                {
                    g.lives++;
                }
            }
            else
            {
                // only a hit while vulnerable loses a life and starts the invincibility and slow timers,
                // touching an obstacle while invincible does nothing
                if (g.obstacleCollisionTimer <= 0.f && g.lives > 0)
                {
                    g.lives--;
                    g.obstacleCollisionTimer = obstacleCollisionCoolDown;
                    g.slowTimer = slowDuration;
                }
                ++count;
            }
        }
        else
        {
            ++count;
        }
    }
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
        if (SpotOverlapsObstacle(g, FloatRect(newObstacle.xpos, newObstacle.ypos, obstacleWidth, obstacleHeight)))
        {
            return;
        }
        g.obstacles.push_back(newObstacle);
    }
}

//function for spawning a booster at one of the fixed spots
void BoosterSpawn(GameState& g)
{
    uniform_int_distribution<int> chance(0, boosterSpawnChance - 1);
    if (chance(g.randomizer) != 0)
    {
        return;
    }
    uniform_int_distribution<int> randomSpot(0, numBoosterSpots - 1);
    int spot = randomSpot(g.randomizer);
    Obstacle newBooster;
    newBooster.xpos = boosterSpots[spot].x;
    newBooster.ypos = boosterSpots[spot].y;
    newBooster.isBooster = true;
    if (SpotOverlapsObstacle(g, FloatRect(newBooster.xpos, newBooster.ypos, boosterWidth, boosterHeight)))
    {
        return;
    }
    g.obstacles.push_back(newBooster);
}

//function for spawning a life at one of the fixed life spots (much rarer than boosters)
void LifeSpawn(GameState& g)
{
    uniform_int_distribution<int> chance(0, lifeSpawnChance - 1);
    if (chance(g.randomizer) != 0)
    {
        return;
    }
    uniform_int_distribution<int> randomSpot(0, numLifeSpots - 1);
    int spot = randomSpot(g.randomizer);
    Obstacle newLife;
    newLife.xpos = lifeSpots[spot].x;
    newLife.ypos = lifeSpots[spot].y;
    newLife.isLife = true;
    if (SpotOverlapsObstacle(g, FloatRect(newLife.xpos, newLife.ypos, lifeWidth, lifeHeight)))
    {
        return;
    }
    g.obstacles.push_back(newLife);
}

//function for spawning a monster at one of the fixed monster spots (as rare as a life)
void MonsterSpawn(GameState& g)
{
    uniform_int_distribution<int> chance(0, monsterSpawnChance - 1);
    if (chance(g.randomizer) != 0)
    {
        return;
    }

    uniform_int_distribution<int> randomSpot(0, numMonsterSpots - 1);
    int spot = randomSpot(g.randomizer);
    Monster newMonster;
    newMonster.xpos = monsterSpots[spot].x;
    newMonster.ypos = monsterSpots[spot].y;
    FloatRect spotRect(newMonster.xpos, newMonster.ypos, monsterWidth, monsterHeight);

    if (SpotOverlapsObstacle(g, spotRect))
    {
        return;
    }

    for (int i = 0; i < int(g.monsters.size()); ++i)
    {
        if (spotRect.intersects(FloatRect(g.monsters[i].xpos, g.monsters[i].ypos, monsterWidth, monsterHeight)))
        {
            return;
        }
    }
    g.monsters.push_back(newMonster);
}

//function for scrolling the obstacles along with the water tiles
void ObstacleScroll(GameState& g, float distanceX, float distanceY)
{
    for (int i = 0; i < int(g.obstacles.size()); ++i)
    {
        g.obstacles[i].xpos += distanceX;
        g.obstacles[i].ypos += distanceY;
    }
    for (int i = 0; i < int(g.monsters.size()); ++i)
    {
        g.monsters[i].xpos += distanceX;
        g.monsters[i].ypos += distanceY;
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

//function to update the monsters, start chasing, chase, die on obstacles, kill the player, cleanup
void MonsterUpdate(GameState& g, FloatRect playerBounds, float speedFactor, float frameTime)
{
    float playerX = playerBounds.left + playerBounds.width / 2.f;
    float playerY = playerBounds.top + playerBounds.height / 2.f;
    int count = 0;
    while (count < int(g.monsters.size()))
    {
        Monster& m = g.monsters[count];

        float dx = playerX - (m.xpos + monsterWidth / 2.f);
        float dy = playerY - (m.ypos + monsterHeight / 2.f);

        if (m.isChasing == false && abs(dx) < width / 2.f && dy > 0.f && dy < height / 2.f)
        {
            m.isChasing = true;
        }

        if (m.isChasing)
        {
            float dist = sqrt(dx * dx + dy * dy);
            if (dist > 0.f)
            {
                float speed = monsterSpeedMultiplier * WaterSpeed * speedFactor;
                m.xpos += dx / dist * speed * frameTime;
                m.ypos += dy / dist * speed * frameTime;
            }
        }
        FloatRect monsterRect(m.xpos, m.ypos, monsterWidth, monsterHeight);

        bool hitObstacle = false;
        if (m.isChasing)
        {
            for (int i = 0; i < int(g.obstacles.size()); ++i)
            {
                if (g.obstacles[i].isBooster == false && g.obstacles[i].isLife == false && monsterRect.intersects(ObstacleBounds(g.obstacles[i])))
                {
                    hitObstacle = true;
                }
            }
        }
        if (hitObstacle)
        {
            g.monsters.erase(g.monsters.begin() + count);
            continue;
        }

        if (monsterRect.intersects(playerBounds))
        {
            g.lives = 0;
        }

        if (m.ypos < monsterODL)
        {
            g.monsters.erase(g.monsters.begin() + count);
        }
        else
        {
            ++count;
        }
    }
}

//function to reset the game after game over (keeps the random number generator as it is)
void ResetGame(GameState& g)
{
    g.waterMap = WaterMap();
    g.scrolling = false;
    g.Direction = 0;
    g.obstacles.clear();
    g.monsters.clear();
    g.boostTimer = 0.f;
    g.obstacleCollisionTimer = 0.f;
    g.slowTimer = 0.f;
    g.lives = startingLives;
    g.distance = 0.f;
}

//function to read the saved scores from the text file into a vector
void LoadScores(vector<int>& scores)
{
    scores.clear();
    ifstream file("highscores.txt");
    int value;
    while (file >> value)
    {
        scores.push_back(value);
    }
}

//bubble sort, puts the highest score at the start of the vector
void BubbleSort(vector<int>& scores)
{
    for (int i = 0; i < int(scores.size()) - 1; ++i)
    {
        for (int j = 0; j < int(scores.size()) - 1 - i; ++j)
        {
            if (scores[j] < scores[j + 1])
            {
                int temp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = temp;
            }
        }
    }
}

//function to write the scores back into the text file
void SaveScores(vector<int> const& scores)
{
    ofstream file("highscores.txt");
    for (int i = 0; i < int(scores.size()); ++i)
    {
        file << scores[i] << "\n";
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
    
    //setting up details for obstacles
    RectangleShape obstacleTile(Vector2f(obstacleWidth, obstacleHeight));
    obstacleTile.setFillColor(obstacleColor);
   
    //setting up the purple square used to draw monsters
    RectangleShape monsterTile(Vector2f(monsterWidth, monsterHeight));
    monsterTile.setFillColor(monsterColor);
   
    //setting up the yellow triangle used to draw boosters
    ConvexShape boosterTile(3);
    boosterTile.setPoint(0, Vector2f(boosterWidth / 2.f, 0.f));
    boosterTile.setPoint(1, Vector2f(boosterWidth, boosterHeight));
    boosterTile.setPoint(2, Vector2f(0.f, boosterHeight));
    boosterTile.setFillColor(boosterColor);
   
    //setting up the red circle used to draw lives
    CircleShape lifeTile(lifeWidth / 2.f);
    lifeTile.setFillColor(lifeColor);
    lifeTile.setOutlineColor(Color::White);
    lifeTile.setOutlineThickness(2.f);
  
    //setting up the life counter text at the top middle of the screen
    Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/pala.ttf"))
    {
        return -1;
    }
    Text lifeText;
    lifeText.setFont(font);
    lifeText.setCharacterSize(32);
    lifeText.setFillColor(Color::White);
    lifeText.setOutlineColor(Color::Black);
    lifeText.setOutlineThickness(2.f);
    lifeText.setPosition(width / 2.f, 10.f); // x is the horizontal center, the origin is set every frame below
   
    //distance text, centered directly below the lives text
    Text distanceText;
    distanceText.setFont(font);
    distanceText.setCharacterSize(32);
    distanceText.setFillColor(Color::White);
    distanceText.setOutlineColor(Color::Black);
    distanceText.setOutlineThickness(2.f);
    distanceText.setPosition(width / 2.f, 50.f); // 40 pixels below the lives text
   
    //setting up the game over text
    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(64);
    gameOverText.setFillColor(Color::White);
    gameOverText.setOutlineColor(Color::Black);
    gameOverText.setOutlineThickness(3.f);
    gameOverText.setString("Musa Died\nPress Enter to restart");
    FloatRect goBounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin(goBounds.left + goBounds.width / 2.f, goBounds.top + goBounds.height / 2.f);
    gameOverText.setPosition(width / 2.f, height / 2.f);
  
    //loading the saved high scores and sorting them
    vector<int> highScores;
    LoadScores(highScores);
    BubbleSort(highScores);
    int bestScore = 0;
    if (highScores.size() > 0)
    {
        bestScore = highScores[0];
    }
 
    //best score text at the top right
    Text bestText;
    bestText.setFont(font);
    bestText.setCharacterSize(32);
    bestText.setFillColor(Color::White);
    bestText.setOutlineColor(Color::Black);
    bestText.setOutlineThickness(2.f);
    bestText.setPosition(width - 220.f, 10.f);
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
            if (event.type == Event::KeyPressed)
            {
                if (gameState.lives <= 0)
                {
                    // game over: only Enter does anything, and it resets the game
                    if (event.key.code == Keyboard::Enter)
                    {
                        ResetGame(gameState);
                    }
                }
                else
                {
                    // now the horizontal direction movement is checked through event KeyPressed so direction is set once
                    gameState.Direction = horizontalDirection(event.key.code, gameState.Direction, gameState.scrolling);
                }
            }
        }
    
        // the game only updates while the player still has lives, otherwise everything freezes
        if (gameState.lives > 0)
        {
            //calling the spawn and removal functions to spawn and remove obstacles from window
            if (gameState.scrolling)
            {
                ObstacleSpawn(gameState);
                BoosterSpawn(gameState);
                LifeSpawn(gameState);
                MonsterSpawn(gameState);
            }
            ObstacleRemoval(gameState);
            // the player's rectangle, built once and used for both the obstacles and the monsters
            FloatRect playerBounds(shape.getPosition().x, shape.getPosition().y, radius * 2.f, radius * 2.f);
            ObstacleCollision(gameState, playerBounds);
            
            float speedFactor = 1.f;
            if (gameState.distance / 10.f >= speedUpDistance)
            {
                speedFactor = speedUpMultiplier;
            }
            float currentSpeed = WaterSpeed * speedFactor;
            if (gameState.boostTimer > 0.f)
            {
                currentSpeed = WaterSpeed * speedFactor * boostSpeedMultiplier;
                gameState.boostTimer -= frameTime;
            }
            if (gameState.slowTimer > 0.f) // the speed reduction comes from slowTimer
            {
                currentSpeed = WaterSpeed * speedFactor * obstacleSpeedReducer;
                gameState.slowTimer -= frameTime;
            }
            if (gameState.obstacleCollisionTimer > 0.f) // this timer only counts down invincibility, it doesn't change the speed
            {
                gameState.obstacleCollisionTimer -= frameTime;
            }
        
            if (gameState.scrolling) // if map is scrolling only then the horizontal and vertical movement functions are called to move the watertiles and the obstacles along with them
            {
                float distanceX = -gameState.Direction * currentSpeed * frameTime;
                float distanceY = -currentSpeed * frameTime;
                gameState.waterMap.VerticalWaterMovement = verticalMapMovement(gameState.waterMap.VerticalWaterMovement, currentSpeed, frameTime);
                gameState.waterMap.HorizontalWaterMovement = horizontalMapMovement(gameState.waterMap.HorizontalWaterMovement, gameState.Direction, currentSpeed, frameTime);
                gameState.distance += currentSpeed * frameTime;
                ObstacleScroll(gameState, distanceX, distanceY);
            }
            // monsters chase even if the player is not scrolling, speedFactor makes them faster after 500m
            MonsterUpdate(gameState, playerBounds, speedFactor, frameTime);
            // if the player just died by either obstacle or monster, save the score (this runs only once per death)
            if (gameState.lives <= 0)
            {
                highScores.push_back(int(gameState.distance / 10.f));
                BubbleSort(highScores);
                if (highScores.size() > 10)
                {
                    highScores.resize(10); // keep only the top 10
                }
                SaveScores(highScores);
                bestScore = highScores[0];
            }
        }
      
        //updating the life counter, distance and best score text
        lifeText.setString("Lives: " + to_string(gameState.lives));
        distanceText.setString(to_string(int(gameState.distance / 10.f)) + "m");
        bestText.setString("Best: " + to_string(bestScore));
     
        //re-centering the lives and distance text horizontally since their width changes with the text
        FloatRect lifeBounds = lifeText.getLocalBounds();
        lifeText.setOrigin(lifeBounds.left + lifeBounds.width / 2.f, 0.f);
        FloatRect distBounds = distanceText.getLocalBounds();
        distanceText.setOrigin(distBounds.left + distBounds.width / 2.f, 0.f);
     
        window.clear(Color(30, 80, 140));  // screen color set to a blue shade
        
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
      
        // following loop draws the obstacles, boosters and lives on the screen
        for (int i = 0; i < int(gameState.obstacles.size()); ++i)
        {
            if (gameState.obstacles[i].isBooster)
            {
                boosterTile.setPosition(gameState.obstacles[i].xpos, gameState.obstacles[i].ypos);
                window.draw(boosterTile);
            }
            else if (gameState.obstacles[i].isLife)
            {
                lifeTile.setPosition(gameState.obstacles[i].xpos, gameState.obstacles[i].ypos);
                window.draw(lifeTile);
            }
            else
            {
                obstacleTile.setPosition(gameState.obstacles[i].xpos, gameState.obstacles[i].ypos);
                window.draw(obstacleTile);
            }
        }

        //drawing monster
        for (int i = 0; i < int(gameState.monsters.size()); ++i)
        {
            monsterTile.setPosition(gameState.monsters[i].xpos, gameState.monsters[i].ypos);
            window.draw(monsterTile);
        }
     
        window.draw(shape);
        window.draw(lifeText);
        window.draw(distanceText);
        window.draw(bestText);
       
        //draws the end screen on top of everything when the player has no lives left
        if (gameState.lives <= 0)
        {
            RectangleShape overlay;
            overlay.setSize(Vector2f(static_cast<float>(width), static_cast<float>(height)));
            overlay.setFillColor(Color(0, 0, 0, 150));
            window.draw(overlay);
            window.draw(gameOverText);
        }
       
        window.display();
    }
    return 0;
}