#include "labyrinth.h"
#include "drawutilities.h"

#include <iostream>

const float Labyrinth::horizontalPropability = 0.4f;
const float Labyrinth::verticalPropability = 0.55f;

Labyrinth::Labyrinth(int width, int height) : width(width), height(height) {
  generate();

  int steps = 1;
  while(!solve()) {
    generate();
    steps++;
  }

  if (steps > 1)
    std::cout << "Needed " << steps << " trys to generate labyrinth" << std::endl;

  //firstStep();

}

Labyrinth Labyrinth::generateLabyrinth(int minWidth, int maxWidth, int minHeight, int maxHeight, float screenRatio) {
  int width = minWidth + (std::rand() % static_cast<int>(maxWidth - minWidth + 1));
  int height = minHeight + (std::rand() % static_cast<int>(maxHeight - minHeight + 1));

  if (screenRatio > 0.0f) {
    if (width / screenRatio >= minHeight && width / screenRatio <= minHeight) {
      height = width / screenRatio;
    } else if (height * screenRatio >= minHeight && height * screenRatio <= maxHeight) {
      width = height * screenRatio;
    }
  }

  return Labyrinth(width, height);
}

void Labyrinth::draw(sf::RenderTarget& surface) {

  // set basic parameters
  float windowWidth  = surface.getSize().x;
  float windowHeight = surface.getSize().y;
  int sqWidth = windowWidth / (width+1);
  int sqHeight = windowHeight / (height+1);
  int xOffset = sqWidth / 2;
  int yOffset = sqHeight / 2;
  int borderWidth = 1;
  int sqToBorderRation = 24;

  // compenstate for different ascpet ratios
  if (sqWidth > sqHeight) {
    sqWidth = sqHeight;
    borderWidth = sqWidth / sqToBorderRation;
    if (borderWidth < 1) borderWidth = 1;
    xOffset = (borderWidth/2) + ((windowWidth - (sqWidth * (width)))/2);
    yOffset += borderWidth/2;
  } else if (sqHeight > sqWidth) {
    sqHeight = sqWidth;
    borderWidth = sqWidth / sqToBorderRation;
    if (borderWidth < 1) borderWidth = 1;
    xOffset += borderWidth/2;
    yOffset = (borderWidth/2) + ((windowHeight - (sqHeight * (height)))/2);
  } else {
    borderWidth = sqWidth / sqToBorderRation;
    xOffset += borderWidth/2;
    yOffset += borderWidth/2;
  }

  // draw horizontal lines
  sf::RectangleShape horizontalBorderShape(sf::Vector2f(sqWidth, borderWidth));
  horizontalBorderShape.setFillColor(sf::Color(0xff, 0xff, 0xff));
  for (unsigned int yy = 0; yy < horizontalBorders.size(); yy++) {
    for (unsigned int xx = 0; xx < horizontalBorders[yy].size(); xx++) {
      if (horizontalBorders[yy][xx]) {
        horizontalBorderShape.setPosition(xx*sqWidth + xOffset, yy*sqHeight + yOffset);
        surface.draw(horizontalBorderShape);
      }
    }
  }

  // draw vertical lines
  sf::RectangleShape verticalBorderShape(sf::Vector2f(borderWidth, sqHeight));
  verticalBorderShape.setFillColor(sf::Color(0xff, 0xff, 0xff));
  for (unsigned int yy = 0; yy < verticalBorders.size(); yy++) {
    for (unsigned int xx = 0; xx < verticalBorders[yy].size(); xx++) {
      if (verticalBorders[yy][xx]) {
        verticalBorderShape.setPosition(xx*sqWidth + xOffset, yy*sqHeight + yOffset);
        surface.draw(verticalBorderShape);
      }
    }
  }

  //
  sf::Text text;
  text.setFont(Draw::arial);
  text.setCharacterSize(sqHeight*0.6);
  text.setFillColor(sf::Color::White);
  int textOffsetY = Draw::arial.getUnderlinePosition(sqHeight*0.6) + Draw::arial.getUnderlineThickness(sqHeight*0.6);
  for (unsigned int yy = 0; yy < solution.size(); yy++) {
    for (unsigned int xx = 0; xx < solution[yy].size(); xx++) {
      if (solution[yy][xx] > 0) {
        text.setString(std::to_string(solution[yy][xx]));
        sf::FloatRect bounds = text.getLocalBounds();
        text.setPosition(xx * sqWidth + xOffset +  sqWidth/2 - bounds.width / 2, (yy * sqHeight) + yOffset + (sqHeight/2) - (bounds.height/2) - textOffsetY);
        surface.draw(text);
      }
    }
  }

}

void Labyrinth::generate() {
  horizontalBorders.clear();
  verticalBorders.clear();
  stepCounter = 0;

  // generate horizontal borders
  for (int yy = 0; yy < height+1; yy++) {
    std::vector<bool> line;
    for (int xx = 0; xx < width; xx++) {
      float random = static_cast<float>(rand() / static_cast<float>(RAND_MAX));
      line.push_back(random < horizontalPropability);
    }
    horizontalBorders.push_back(line);
  }

  // generate vertical borders
  for (int yy = 0; yy < height; yy++) {
    std::vector<bool> line;
    for (int xx = 0; xx < width + 1; xx++) {
      float random = static_cast<float>(rand() / static_cast<float>(RAND_MAX));
      line.push_back(random < verticalPropability);
    }
    verticalBorders.push_back(line);
  }
}

int Labyrinth::nextStep() {

  int progress = -1;

  if (stepCounter == 0) {
    firstStep();
  } else {
    std::vector<Field> nextStep;
    stepCounter++;

    for (Field& step : solutionStep) {
      // left
      if (step.x > 0 && verticalBorders[step.y][step.x] == false && solution[step.y][step.x-1] == 0) {
        nextStep.push_back(Field(step.x-1, step.y, stepCounter));
        solution[step.y][step.x-1] = stepCounter;
      }
      // right
      if (step.x < width-1 && verticalBorders[step.y][step.x+1] == false && solution[step.y][step.x+1] == 0) {
        nextStep.push_back(Field(step.x+1, step.y, stepCounter));
        solution[step.y][step.x+1] = stepCounter;
      }
      // top
      if (step.y > 0 && horizontalBorders[step.y][step.x] == false && solution[step.y-1][step.x] == 0) {
        nextStep.push_back(Field(step.x, step.y-1, stepCounter));
        solution[step.y-1][step.x] = stepCounter;
      }
      // bottom
      if (step.y < height-1 && horizontalBorders[step.y+1][step.x] == false && solution[step.y+1][step.x] == 0) {
        nextStep.push_back(Field(step.x, step.y+1, stepCounter));
        solution[step.y+1][step.x] = stepCounter;
      }
    }

    solutionStep = nextStep;
  }

  for (Field& step : solutionStep) {
    if (step.y > progress) progress = step.y;
    if (step.y == height-1 && horizontalBorders[step.y+1][step.x] == false) {
      progress++;
      break;
    }
  }

  return progress;

}

void Labyrinth::firstStep() {
  solution.clear();
  solutionStep.clear();

  // init fields
  for (int yy = 0; yy < height; yy++) {
    std::vector<int> line;
    for (int xx = 0; xx < width; xx++) {
      line.push_back(0);
    }
    solution.push_back(line);
  }

  // set start position
  for (int xx = 0; xx < width; xx++) {
    if (horizontalBorders[0][xx] == false) {
      solution[0][xx] = 1;
      solutionStep.push_back(Field(xx, 0, 1));
    }
  }

  stepCounter = 1;
}

bool Labyrinth::solve() {
  int progress = 0;
  while (progress > -1 && progress < height) {
    progress = nextStep();
  }

  if (progress >= 0)
    generateSolutionPath();

  return progress >= 0;
}

void Labyrinth::generateSolutionPath () {
  solutionPath.clear();

  std::vector<sf::Vector2i> currentFields;

  // find end points
  for (int xx = 0; xx < width; xx++) {
    if (solution[height-1][xx] == stepCounter && horizontalBorders[height][xx] == false) {
      solutionPath.push_back(sf::Vector2i(xx, height-1));
      currentFields.push_back(sf::Vector2i(xx, height-1));
    }
  }

  // iterate backwards
  for (int ii = stepCounter; ii > 0; ii--) {
    std::vector<sf::Vector2i> newFields;

    for (sf::Vector2i field : currentFields) {
      // left
      if (field.x > 0 && solution[field.y][field.x-1] == ii-1 && verticalBorders[field.y][field.x] == false)
        if (std::find(newFields.begin(), newFields.end(), sf::Vector2i(field.x-1, field.y)) == newFields.end())
          newFields.push_back(sf::Vector2i(field.x-1, field.y));
      // right
      if (field.x < width-1 && solution[field.y][field.x+1] == ii-1 && verticalBorders[field.y][field.x+1] == false)
        if (std::find(newFields.begin(), newFields.end(), sf::Vector2i(field.x+1, field.y)) == newFields.end())
          newFields.push_back(sf::Vector2i(field.x+1, field.y));
      // bottom
      if (field.y < height-1 && solution[field.y+1][field.x] == ii-1 && horizontalBorders[field.y+1][field.x] == false)
        if (std::find(newFields.begin(), newFields.end(), sf::Vector2i(field.x, field.y+1)) == newFields.end())
          newFields.push_back(sf::Vector2i(field.x, field.y+1));
      // top
      if (field.y > 0 && solution[field.y-1][field.x] == ii-1 && horizontalBorders[field.y][field.x] == false)
        if (std::find(newFields.begin(), newFields.end(), sf::Vector2i(field.x, field.y-1)) == newFields.end())
          newFields.push_back(sf::Vector2i(field.x, field.y-1));
    }

    for (sf::Vector2i field : newFields)
      solutionPath.push_back(field);

    currentFields = newFields;
  }

}
