#include "labyrinthrenderer.h"
#include "drawutilities.h"

LabyrinthRenderer::LabyrinthRenderer(const Labyrinth& labyrinth) : labyrinth(labyrinth) {

  for(int yy = 0; yy < labyrinth.getHeight(); yy++) {
    std::vector<sf::Color> line;
    for (int xx = 0; xx < labyrinth.getHeight(); xx++) {
      line.push_back(sf::Color(0, 0, 0));
    }
    fields.push_back(line);
  }
}

void LabyrinthRenderer::draw(sf::RenderTarget& surface) const {

  // set basic parameters
  float windowWidth  = surface.getSize().x;
  float windowHeight = surface.getSize().y;
  int sqWidth = windowWidth / (labyrinth.getWidth()+1);
  int sqHeight = windowHeight / (labyrinth.getHeight()+1);
  int xOffset = sqWidth / 2;
  int yOffset = sqHeight / 2;
  int borderWidth = 1;
  int sqToBorderRation = 24;
  const std::vector<std::vector<bool>>& horizontalBorders = labyrinth.getHorizontalBorders();
  const std::vector<std::vector<bool>>& verticalBorders = labyrinth.getVerticalBorders();

  // compenstate for different ascpet ratios
  if (sqWidth > sqHeight) {
    sqWidth = sqHeight;
    borderWidth = sqWidth / sqToBorderRation;
    if (borderWidth < 1) borderWidth = 1;
    xOffset = (borderWidth/2) + ((windowWidth - (sqWidth * (labyrinth.getWidth())))/2);
    yOffset += borderWidth/2;
  } else if (sqHeight > sqWidth) {
    sqHeight = sqWidth;
    borderWidth = sqWidth / sqToBorderRation;
    if (borderWidth < 1) borderWidth = 1;
    xOffset += borderWidth/2;
    yOffset = (borderWidth/2) + ((windowHeight - (sqHeight * (labyrinth.getHeight())))/2);
  } else {
    borderWidth = sqWidth / sqToBorderRation;
    xOffset += borderWidth/2;
    yOffset += borderWidth/2;
  }

  // draw colored fields
  sf::RectangleShape fieldShape(sf::Vector2f(sqWidth, sqHeight));
  for (unsigned int yy = 0; yy < fields.size(); yy++) {
    for (unsigned int xx = 0; xx < fields.size(); xx++) {
      fieldShape.setPosition(xx*sqWidth + xOffset, yy*sqHeight + yOffset);
      fieldShape.setFillColor(fields[yy][xx]);
      surface.draw(fieldShape);
    }
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

  // draw numbers
  if (false) {
    const std::vector<std::vector<int>>& solution = labyrinth.getSolution();
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
}

void LabyrinthRenderer::startAnimation() {
  animationRunning = true;
  animationThread = std::thread(&LabyrinthRenderer::animate, this);
}

void LabyrinthRenderer::stopAnimation() {
  animationRunning = false;
  animationThread.join();
}

void LabyrinthRenderer::resetAnimation() {
  steps = 0;
  animationState = AnimationState::ANIMATE;
  fields.clear();
  for(int yy = 0; yy < labyrinth.getHeight(); yy++) {
    std::vector<sf::Color> line;
    for (int xx = 0; xx < labyrinth.getHeight(); xx++) {
      line.push_back(sf::Color(0, 0, 0));
    }
    fields.push_back(line);
  }
}

void LabyrinthRenderer::animate() {

  while(animationRunning) {

    if (animationState == AnimationState::ANIMATE) {
      animationState = animateSolving();
      steps++;
      if (animationState == AnimationState::SHOW_SOLUTION) {
        steps = 0;
      }
    } else if (animationState == AnimationState::SHOW_SOLUTION) {
      animationState = animateSolution();
      steps++;
    }
  }
}

LabyrinthRenderer::AnimationState LabyrinthRenderer::animateSolving() {

  animationMutex.lock();

  dullOutFields();

  for (int yy = 0; yy < labyrinth.getHeight(); yy++) {
    for ( int xx = 0; xx < labyrinth.getWidth(); xx++) {
      if (labyrinth.getSolution()[yy][xx] == steps && steps > 0) {
        fields[yy][xx] = sf::Color(128, 128, 128);
      }
    }
  }

  AnimationState state = AnimationState::ANIMATE;
  if (steps >= labyrinth.getSolutionSteps())
    state = AnimationState::SHOW_SOLUTION;

  animationMutex.unlock();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000/40));

  return state;
}

LabyrinthRenderer::AnimationState LabyrinthRenderer::animateSolution() {

  animationMutex.lock();

  dullOutFields();

  const std::vector<sf::Vector2i>& solution = labyrinth.getSolutionPath();

  for (sf::Vector2i step : solution) {
    fields[step.y][step.x] = sf::Color::White;
  }

  animationMutex.unlock();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000/20));

  if (steps < 128/10)
    return AnimationState::SHOW_SOLUTION;
  else
    return AnimationState::DONE;
}

void LabyrinthRenderer::dullOutFields() {
  int vanishingStep = 10;
  for (int yy = 0; yy < labyrinth.getHeight(); yy++) {
    for ( int xx = 0; xx < labyrinth.getWidth(); xx++) {
      if (fields[yy][xx] != sf::Color::Black) {
        sf::Color& color = fields[yy][xx];
        if (color.r >= vanishingStep) color.r -= vanishingStep;
        else color.r = 0;
        if (color.g >= vanishingStep) color.g -= vanishingStep;
        else color.g = 0;
        if (color.b >= vanishingStep) color.b -= vanishingStep;
        else color.b = 0;
      }
    }
  }
}
