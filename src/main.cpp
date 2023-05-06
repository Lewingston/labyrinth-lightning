#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include <thread>

#include "labyrinth.h"
#include "labyrinthrenderer.h"
#include "drawutilities.h"

int main (int /*argc*/, char** /*args*/) {

  // application settings
  constexpr int windowWidth  = 1200;
  constexpr int windowHeight = 1200;
  constexpr int minWidth = 50;
  constexpr int maxWidth = 50;
  constexpr int minHeight = 50;
  constexpr int maxHeight = 50;
  constexpr float screenRatio = static_cast<float>(windowWidth) / windowHeight;

  const int screenWidth = sf::VideoMode::getDesktopMode().width;
  const int screenHeight = sf::VideoMode::getDesktopMode().height;

  sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Lightning", sf::Style::Close | sf::Style::Titlebar);

  // Center window in the middle of the screen
  window.setPosition(sf::Vector2i((screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2));
  window.setFramerateLimit(60);

  Draw::init();

  Labyrinth labyrinth = Labyrinth::generateLabyrinth(minWidth, maxWidth, minHeight, maxHeight, screenRatio);
  LabyrinthRenderer labyrinthRenderer(labyrinth);
  labyrinthRenderer.startAnimation();

  std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
  float frameRate = 0.0f;
  sf::Text frameRateText;
  frameRateText.setFont(Draw::arial);
  frameRateText.setCharacterSize(16);
  frameRateText.setFillColor(sf::Color(200, 200, 200));

  while(window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::KeyPressed) {

      }
    }

    window.clear(sf::Color::Black);

    labyrinthRenderer.animationMutex.lock();

    if (labyrinthRenderer.isAnimationDone()) {
      labyrinth = Labyrinth::generateLabyrinth(minWidth, maxWidth, minHeight, maxHeight, screenRatio);
      labyrinthRenderer.resetAnimation();
    }

    labyrinthRenderer.draw(window);

    labyrinthRenderer.animationMutex.unlock();

    // show frame rate
    if (false) {
      frameRateText.setString(std::to_string(static_cast<int>(frameRate)));
      window.draw(frameRateText);
    }

    // calculate frame rate
    std::chrono::steady_clock::time_point newTime = std::chrono::steady_clock::now();
    int timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(newTime - time).count();
    if (timeDiff > 0)
      frameRate = (1000 / timeDiff);
    time = newTime;

    window.display();
  }

  return 0;
}
