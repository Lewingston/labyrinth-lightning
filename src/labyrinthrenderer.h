#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <mutex>

#include "labyrinth.h"

class LabyrinthRenderer {

  public:

    LabyrinthRenderer(const Labyrinth& labyrinth);
    virtual ~LabyrinthRenderer() = default;

    void draw(sf::RenderTarget& surface) const;
    void startAnimation();
    void stopAnimation();
    bool isAnimationDone() {return animationState == AnimationState::DONE;}
    void resetAnimation();

    std::mutex animationMutex;

  private:

    enum class AnimationState {
      ANIMATE,
      SHOW_SOLUTION,
      DONE
    };

    const Labyrinth& labyrinth;

    std::vector<std::vector<sf::Color>> fields;

    std::thread animationThread;

    AnimationState animationState = AnimationState::ANIMATE;
    bool animationRunning = false;
    int steps = 0;

    void animate();
    AnimationState animateSolving();
    AnimationState animateSolution();

    void dullOutFields();
};
