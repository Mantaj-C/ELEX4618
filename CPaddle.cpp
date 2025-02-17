#include "stdafx.h"
#include "CPaddle.h"

#define PADDLE_SPEED 15

enum {LEFT = -1, NO, RIGHT};

CPaddle::CPaddle(cv::Size size) {
   _position = cv::Point2f(size.width / 2, size.height - (PADDLE_HEIGHT/2));
   _velocity = cv::Point2f(0, 0);
   _rectangle_size = cv::Rect2f(_position.x - (PADDLE_WIDTH / 2), _position.y - (PADDLE_HEIGHT / 2),
      PADDLE_WIDTH, PADDLE_HEIGHT);

   }

CPaddle::~CPaddle() {

   }

void CPaddle::move(int direction) {
   cv::Point2f temp_velocity = cv::Point2f(direction * PADDLE_SPEED, 0);
   cv::Point2f temp_position = _position;
   if (((temp_position += temp_velocity).x >= (1000 - (PADDLE_WIDTH / 2))) ||
      ((temp_position += temp_velocity).x <= ((PADDLE_WIDTH / 2))))
      _velocity = cv::Point2f(0, 0);
   else
      _velocity = temp_velocity;
   _position += _velocity;
   _rectangle_size = cv::Rect2f(_position.x - (PADDLE_WIDTH / 2), _position.y - (PADDLE_HEIGHT / 2),
      PADDLE_WIDTH, PADDLE_HEIGHT);
   }