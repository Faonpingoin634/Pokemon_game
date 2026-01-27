// main.cpp - Test SFML 3.0.2
#include <SFML/Graphics.hpp>

int main() {
    // Création de la fenêtre
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Projet Pokemon Binome");

    while (window.isOpen()) {
        // Gestion des événements (fermeture)
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Affichage d'un fond bleu (pour prouver que ça marche)
        window.clear(sf::Color::Blue);
        window.display();
    }
    return 0;
}