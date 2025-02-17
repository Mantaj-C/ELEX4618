#include "stdafx.h"
#include "CBricks.h"

#define BRICK_WIDTH 50
#define BRICK_HEIGHT 30

CBricks::CBricks(cv::Size size, cv::Point2f position) {
   _position = position;
   _lives = 1;
   _velocity = cv::Point2f(0, 0);
   _rectangle_size = cv::Rect2f(_position.x - (BRICK_WIDTH / 2), _position.y - (BRICK_HEIGHT / 2),
      BRICK_WIDTH, BRICK_HEIGHT);
   }

CBricks::~CBricks() {

   }