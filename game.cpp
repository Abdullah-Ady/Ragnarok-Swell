#include <SFML/Graphics.hpp>
using namespace sf;

void sidewayMovement(CircleShape& shape, float speed, float fT)
{
    float sM = 0.f;
    //following if statements check if any left keys have been pressed to move circle to the left, and right keys to make it move to the right

    if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right))
    {
        sM += speed;
    }
    if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left))
    {
        sM -= speed;
    }

    shape.move(sM * fT, 0.f);
}

int main() {
    //defining all the constants for circle shape, window size and movements


	int const width = 1280;
	int const height = 720;
    float const downMovement = 60.f;
    float const sideMovement = 300.f;
    float const radius = 32.f;
    //creation of the window

    RenderWindow window(VideoMode(width, height), "Surf");
    window.setVerticalSyncEnabled(true);
    //setting up the circle and all its properties
    CircleShape shape(radius);
    shape.setFillColor(Color::Green);
    shape.setPosition((width / 2.f - radius), (height / 3.f - radius));
    //setting up details of the water map
    Vector2f const TileSize(10.f, 50.f);
    RectangleShape waterTile(TileSize);

    
    Clock clock; //declaring a clock to keep counting time between each frame


    while (window.isOpen()) {

        float frameTime = clock.restart().asSeconds();
        //calculating frame time in seconds, using later for horizontal and vertical distance movement


        //following loop checks if user presses the 'x' or cross button to shut window to close program
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
            {
                window.close();
            }
        }

        shape.move(0.f, downMovement * frameTime); // keeps circle automatically moving downwards

        sidewayMovement(shape, sideMovement, frameTime); // calls the created function to move sideways

        window.clear(Color(30, 80, 140));  // screen color set to a blue shade

        //following loop draws water tiles to show details of map
        for (int column = 0; column < 16; column++)
        {
            for (int row = 0; row < 9; row++)
            {
                if ((row + column) % 2 == 0)
                {
                    waterTile.setFillColor(Color(0, 128, 128));
                }
                else
                {
                    waterTile.setFillColor(Color(65, 105, 225));
                }
                waterTile.setPosition(column * 80.f, row * 80.f);
                window.draw(waterTile);
            }
        }
        window.draw(shape);
        window.display();
    }

    return 0;
}
