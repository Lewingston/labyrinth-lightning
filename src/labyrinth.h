#pragma once

#include <SFML/Graphics.hpp>

class Labyrinth {

  public:

    Labyrinth(int width, int height);
    virtual ~Labyrinth() = default;

    static Labyrinth generateLabyrinth(int minWidth, int maxWidth, int minHeight, int maxHeight, float screenRatio);

    void draw(sf::RenderTarget& surface);

    int nextStep();

    int getWidth() const {return width;}
    int getHeight() const {return height;}

    const std::vector<std::vector<bool>>& getHorizontalBorders() const {return horizontalBorders;}
    const std::vector<std::vector<bool>>& getVerticalBorders() const {return verticalBorders;}
    const std::vector<std::vector<int>>& getSolution() const {return solution;}

    int getSolutionSteps() const {return stepCounter;}
    const std::vector<sf::Vector2i>& getSolutionPath() const {return solutionPath;}

  private:

    struct Field {
      Field(int x, int y, int step) : x(x), y(y), step(step) {};
      int x;
      int y;
      int step;
    };

    static const float horizontalPropability;
    static const float verticalPropability;

    int width  = 0;
    int height = 0;

    std::vector<std::vector<bool>> horizontalBorders;
    std::vector<std::vector<bool>> verticalBorders;
    std::vector<std::vector<int>> solution;
    std::vector<Field> solutionStep;
    std::vector<sf::Vector2i> solutionPath;
    int stepCounter = 0;

    void generate();
    void firstStep();
    bool solve();
    void generateSolutionPath();

};
