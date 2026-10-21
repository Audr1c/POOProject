// Test minimal SFML 2.6 : une balle qui tombe et rebondit, reliee a un point fixe.
// Clic gauche : replace la balle sous la souris. Fermer la fenetre pour quitter.
#include <SFML/Graphics.hpp>
#include <algorithm>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Test SFML 2.6");
    window.setFramerateLimit(60);

    const float radius = 20.f;
    sf::CircleShape ball(radius);
    ball.setOrigin(radius, radius);
    ball.setFillColor(sf::Color(255, 140, 0));

    const sf::Vector2f anchor(400.f, 50.f);
    sf::Vector2f pos(400.f, 100.f);
    float vy = 0.f;
    const float g = 900.f;   // pixels / s^2 (juste pour le test)

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                pos = sf::Vector2f(static_cast<float>(event.mouseButton.x),
                                   static_cast<float>(event.mouseButton.y));
                vy = 0.f;
            }
        }

        const float dt = std::min(clock.restart().asSeconds(), 0.05f);
        vy += g * dt;
        pos.y += vy * dt;
        if (pos.y > 600.f - radius) {      // rebond sur le sol
            pos.y = 600.f - radius;
            vy = -0.8f * vy;
        }
        ball.setPosition(pos);

        const sf::Vertex line[] = { sf::Vertex(anchor, sf::Color::White),
                                    sf::Vertex(pos,    sf::Color::White) };

        window.clear(sf::Color(30, 30, 40));   // ~ Renderer::clear
        window.draw(line, 2, sf::Lines);       // ~ Renderer::drawLine
        window.draw(ball);                     // ~ Renderer::drawCircle
        window.display();                      // ~ Renderer::display
    }
}
