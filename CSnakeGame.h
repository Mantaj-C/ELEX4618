#pragma once
#include "stdafx.h"
#include "CBase4618.h"
#include <vector>


class CSnakeGame : public CBase4618 {
private: 
   struct _color_structure {
      std::string _color;
      cv::Scalar _color_scalar;
      int _color_pin;
      };
   
   cv::Size _size;
   bool _reset;
   int _color_index;
   std::vector<_color_structure> _colorArray;
   std::deque<cv::Point> _snake_position;
   cv::Point2f _joystick_position;
   int _prev_color;
   bool _button_pressed;
   bool _color_flag;
   cv::Point _prev_direction;

public:
   CSnakeGame(cv::Size size);
   ~CSnakeGame();
   void gpio() override;
   void update() override;
   void draw() override;
   };