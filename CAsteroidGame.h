#pragma once

#include "stdafx.h"
#include "CBase4618.h"
#include "cvui.h"
#include "CGameObject.h"
#include "CShip.h"
#include "CMissile.h"
#include "CAsteroid.h"
#include <deque>
#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>

class CAsteroidGame : public CBase4618 {
private:
   cv::Size _size;
   CShip _ship;
   std::deque<CAsteroid> _asteroids;
   std::deque<CMissile> _missiles;
   bool _reset;
   bool _button_pressed;
   bool _game_over;
   float _FPS;
   int _framecount = 0;
   double _lastTime_frame;
   double _lastTime_asteroid;
   int _score;
   float _joystick_position;
   bool _button_pressed;
   int _update_time;
   int _gpio_time;
   int _draw_time;
   bool _missile_fire;
   bool _thrusters;
   bool _hyperspace;

public:
   CAsteroidGame(cv::Size size));
   ~CAsteroidGame();
   void reset();
   };