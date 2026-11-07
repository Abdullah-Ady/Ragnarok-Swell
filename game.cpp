#include <SFML/Graphics.hpp>
using namespace sf;

void sidewayMovement(CircleShape& shape, float speed, float dM)
{
    float sM = 0.f;

    if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right))
    {
        sM += speed;
    }
    if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left))
    {
        sM -= speed;
    }

    shape.move(sM * dM, 0.f);
}

int main() {

	int const width = 1000;
	int const height = 1000;
    float const downMovement = 60.f;
    float const sideMovement = 300.f;

    RenderWindow window(VideoMode(width, height), "Surf");
    CircleShape shape(25.f);
    shape.setFillColor(Color::Green);
    shape.setPosition(width / 2.f, 0.f);

    Clock clock;

    while (window.isOpen()) {

        float dM = clock.restart().asSeconds();

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
            {
                window.close();
            }
        }

        shape.move(0.f, downMovement * dM);
        sidewayMovement(shape, sideMovement, dM);

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}
