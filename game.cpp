#include <SFML/Graphics.hpp>
using namespace sf;

int main() {

    //defining all the constants for circle shape, window size and movements
    int const width = 1280;
    int const height = 720;
    float const downMovement = 60.f;
    float const sideMovement = 300.f;
    float const radius = 32.f;

    //creation of the window
    RenderWindow window(VideoMode(width, height), "Surf");
    window.setVerticalSyncEnabled(true);//used to limit the games frame rate to the pc's refresh rate

    //setting up the circle and all its properties
    CircleShape shape(radius);
    shape.setFillColor(Color::Green);
    shape.setPosition((width / 2.f - radius), (height / 3.f - radius));

    //setting up details of the water map
    Vector2f const TileSize(10.f, 50.f);
    RectangleShape waterTile(TileSize);

    Clock clock;  //declaring a clock to keep counting time between each frame


    //declaring constants and variables for the movement of the map tiles
    float VerticalWaterMovement = 0.f;
    float const WaterSpeed = 150.f;
    float HorizontalWaterMovement = 0.f;
    int Direction;

    while (window.isOpen())
    {

        float frameTime = clock.restart().asSeconds(); //calculating frame time in seconds, using later for horizontal and vertical distance movement


        //following loop checks if user presses the 'x' or cross button to shut window to close program
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
        }

        //as soon as vertical movement causes tiles to move down 160 pixels, they are reset to original position on screen to create an endless map feel
        if (VerticalWaterMovement <= -160.f)
        {
            VerticalWaterMovement += 160.f;
        }
        else
        {
            VerticalWaterMovement -= WaterSpeed * frameTime;
        }

        //left/right and A/D keys used to control horizontal movement of tiles, when nothing is pressed it moves straight down
        if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right))
        {
            Direction = 1;
        }
        else if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left))
        {
            Direction = -1;
        }
        else
        {
            Direction = 0;
        }

        //calculation of the horizontal movement, as soon as the tiles move out of screen, they are reset to original positions
        HorizontalWaterMovement -= Direction * WaterSpeed * frameTime;

        if (HorizontalWaterMovement <= -160.f)
        {
            HorizontalWaterMovement += 160.f;
        }
        if (HorizontalWaterMovement > 0.f)
        {
            HorizontalWaterMovement -= 160.f;
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
                waterTile.setPosition((column * 80.f) + HorizontalWaterMovement, (row * 80.f) + VerticalWaterMovement);
                window.draw(waterTile);
            }
        }
        window.draw(shape);
        window.display();
    }

    return 0;
}
