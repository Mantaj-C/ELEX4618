#pragma once

#include "stdafx.h"
#include "CBase4618.h"
#include "CGameObject.h"
#include "CShip.h"
#include "CMissile.h"
#include "CAsteroid.h"
#include "CInvader.h"
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <deque>
#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <random>

class CAsteroidGame : public CBase4618 {
private:
   cv::Size _size;
   CShip _ship;
   CShip _ship_lives_image;
   std::deque<CAsteroid> _asteroids;
   std::deque<CMissile> _missiles;
   std::deque<CInvader> _invaders;
   std::deque<CMissile> _invaders_missiles;
   bool _reset;
   bool _button_pressed;
   bool _game_over;
   float _FPS;
   int _frameCount;
   double _lastTime_frame;
   double _lastTime_asteroid;
   double _lastTime_invader;
   double _lastTime_invader_fire;
   int _score;
   float _joystick_position;
   int _update_time;
   int _gpio_time;
   int _draw_time;
   bool _missile_fire;
   bool _thrusters;
   bool _hyperspace;
   std::mutex _missiles_mutex;
   bool _controller;
   std::deque<bool> _button_states;
   cv::Mat _asteroid_image;

public:
   CAsteroidGame(cv::Size size);
   ~CAsteroidGame();
   void gpio() override;
   void update() override;
   void draw() override;
   void reset();
   void mini_asteroid_spawn(cv::Point2f postition, cv::Point2f velocity);
   };