#pragma once

#include "stdafx.h"
#include "CBase4618.h"
#include "CBBObjects.h"
#include "CPaddle.h"
#include "CBall.h"
#include "CBricks.h"
#include "Rainbowcolors.h"
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <deque>
#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <random>

class CBrickBreaker : public CBase4618 {
private:
   cv::Size _size;
   CBall _ball;
   CPaddle _paddle;
   std::deque<CBricks> _bricks;
   bool _reset;
   bool _game_over;
   float _FPS;
   int _frameCount;
   double _lastTime_frame;
   int _score;
   float _joystick_position;
   int _update_time;
   int _gpio_time;
   int _draw_time;
   bool _controller;
   bool _button_state;
   std::array<cv::Scalar, 100> _rainbow;
   int level;
   int _rainbow_select;

public:
   CBrickBreaker(cv::Size size);
   ~CBrickBreaker();
   void gpio() override;
   void update() override;
   void draw() override;
   void reset();
   void level_create(int level_select);
   };