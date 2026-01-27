// main.cpp - Test SFML 3.0.2
#include "../include/SFML/Graphics.hpp" // On pointe vers le dossier local

int main() {
    // Création de la fenêtre (Syntaxe SFML 3.0)
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Projet Pokemon Binome");

    while (window.isOpen()) {
        // Gestion des événements
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Fond bleu Pokémon !
        window.clear(sf::Color(100, 149, 237)); // Un bleu un peu plus doux (Cornflower Blue)
        window.display();
    }
    return 0;
}