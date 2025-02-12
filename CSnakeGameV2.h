/**
 * @file CSnakeGame.h
 * @brief Declaration of the CSnakeGame class, providing game logic for a Snake game implementation.
 */

#pragma once

#include "stdafx.h"
#include "CBase4618.h"
#include "RainbowColors.h"
#include <vector>
#include <deque>

 /**
  * @class CSnakeGameV2
  * @brief A class representing the Snake game, extending the CBase4618 class.
  *
  * This class handles the game logic, including rendering, user input (via GPIO and joystick),
  * and LED color synchronization.
  */
class CSnakeGameV2 : public CBase4618 {
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
   cv::Point _direction;
   float _FPS;
   int _frameCount;
   double _lastTime_frame;
   double _lastTime_apple;
   double _lastTime_lazer;
   bool _musicplaying;
   int _step_size;
   int _snake_speed;
   int _snake_length;
   bool _apple_eaten;
   std::deque<cv::Rect> _apple_position;
   std::deque<cv::Point> _apple_direction;
   int _score;
   cv::Rect _snake_hit_box;
   bool _game_over;
   std::mutex _music_mutex;
   int _update_time;
   int _gpio_time;
   int _draw_time;
   cv::Mat _overlay;
   float _apple_spawn_rate;
   std::deque<cv::Point> _lazer_position;
   std::deque<cv::Point> _lazer_direction;
   cv::Mat _bottom_ship;
   cv::Mat _right_ship;
   bool _reset_done;
   bool _lazer_firing;
   bool _lazer_drawn;
   bool _apple_movement;
   bool _lazer_on;
   std::array<cv::Scalar, 100> _rainbow;

public:
   CSnakeGameV2(cv::Size size);
   ~CSnakeGameV2();

   void gpio() override;
   void update() override;
   void draw() override;

   void reset();
   void apple_spawn();
   void snake_vector_logic(bool apple_flag);
   void lazer();
   };
