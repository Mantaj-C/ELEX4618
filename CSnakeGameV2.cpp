#include "stdafx.h"
#include "cvui.h"
#include "CSnakeGameV2.h"
#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>
#include <iostream>
#include <iomanip>

#define GAME_NAME "Mantaj Chauhan Snake Game"
#define SNAKE_THICKNESS 10
#define SCORE_INCREMENT 1
#define APPLE_SPAWN_SPEED 5.0
#define APPLE_SPAWN_FAST 0.2
#define APPLE_RADIUS 10
#define MAX_FPS 30
#define STARTING_LENGTH 100
#define MAX_STEP 20
#define JOYSTICK_X 2
#define JOYSTICK_Y 26
#define SWITCH_1 33
#define SWITCH_2 32
#define RGBLED_RED_PIN 39
#define JOYSTICK_UPPER_THRESHOLD 80.0
#define JOYSTICK_LOWER_THRESHOLD 20.0
#define RGBLED_GREEN_PIN 38
#define RGBLED_BLUE_PIN 37
#define BUZZER 40

enum { DIGITAL = 0, ANALOG, SERVO };

CSnakeGameV2::CSnakeGameV2(cv::Size size)
   : _size(size), _reset(true), _color_flag(true),
   _color_index(0), _joystick_position(0, 0), _button_pressed(false), _musicplaying(false),
   _step_size(10), _snake_speed(100), _game_over(false), _apple_spawn_rate(APPLE_SPAWN_SPEED) {
   _colorArray.push_back({ "Red", cv::Scalar(0,0,255), RGBLED_RED_PIN });
   _colorArray.push_back({ "Green", cv::Scalar(0,255,0), RGBLED_GREEN_PIN });
   _colorArray.push_back({ "Blue", cv::Scalar(255,0,0), RGBLED_BLUE_PIN });
   _colorArray.push_back({ "Rainbow", cv::Scalar(rand() % 256,rand() % 256,rand() % 256), BUZZER });
   _prev_color = _colorArray.size() - 1;
   _snake_length = STARTING_LENGTH;
   _FPS = 0.0f;
   _frameCount = 0;
   _lastTime_frame = cv::getTickCount() / cv::getTickFrequency();
   _lastTime_apple = cv::getTickCount() / cv::getTickFrequency();
   setCanvas(cv::Mat::zeros(size, CV_8UC3));
   cvui::init(GAME_NAME);
   }

CSnakeGameV2::~CSnakeGameV2() {
   getControl().set_data(DIGITAL, RGBLED_BLUE_PIN, 0);
   getControl().set_data(DIGITAL, RGBLED_RED_PIN, 0);
   getControl().set_data(DIGITAL, RGBLED_GREEN_PIN, 0);
   mciSendString("stop myMusic", NULL, 0, NULL);
   mciSendString("close myMusic", NULL, 0, NULL);
   }

void CSnakeGameV2::gpio() {
   while (!getExit()) {
      auto gpio_start = std::chrono::steady_clock::now();//ChatGPT
      int button1_state = 0;
      int button2_state = 0;
      _joystick_position.x = getControl().get_analog(ANALOG, JOYSTICK_X);
      _joystick_position.y = getControl().get_analog(ANALOG, JOYSTICK_Y);
      if (getControl().get_button(SWITCH_2, button1_state)) {
         _button_pressed = true;
         }
      else if (getControl().get_button(SWITCH_1, button2_state)) {
         _reset = true;
         }
      if (_color_flag) {
         getControl().set_data(DIGITAL, _colorArray[_prev_color]._color_pin, 0);
         getControl().set_data(DIGITAL, _colorArray[_color_index]._color_pin, 1);
         _color_flag = false;
         }
      ////////////////////////////////////////////////////////////////////ChatGPT
      auto gpio_end = std::chrono::steady_clock::now();
      _gpio_time = static_cast<int>((std::chrono::duration_cast<std::chrono::microseconds>(gpio_end - gpio_start)).count());
      ////////////////////////////////////////////////////////////////////ChatGPT
      }
   }

void CSnakeGameV2::update() {
   // This loop runs in its own thread.
   while (!getExit()) {
      auto update_start = std::chrono::steady_clock::now(); //ChatGPT
      if (_reset) {
         reset();
         }

      double currentTime = cv::getTickCount() / cv::getTickFrequency();
      if (currentTime - _lastTime_frame >= 1.0) {
         _FPS = _frameCount / (currentTime - _lastTime_frame);
         _frameCount = 0;
         _lastTime_frame = currentTime;
         }
      if (currentTime - _lastTime_apple >= _apple_spawn_rate) {
         _lastTime_apple = currentTime;
         apple_spawn();
         }
      _snake_hit_box = cv::Rect(_snake_position.front().x, _snake_position.front().y, SNAKE_THICKNESS, SNAKE_THICKNESS);

      for (int i = 0; i < _apple_position.size(); i++) {
         if ((_snake_hit_box & _apple_position[i]).area() > 0) {
            _apple_eaten = true;
            _apple_position.erase(_apple_position.begin() + i);
            }
         }
      if (_apple_eaten) {
         if (!_game_over)
         _score = _score + SCORE_INCREMENT;
         snake_vector_logic(_apple_eaten);
         _apple_eaten = false;
         }
      cv::Point head = _snake_position.front();
      _colorArray[3]._color_scalar = cv::Scalar(rand() % 256, rand() % 256, rand() % 256);
      if (_joystick_position.x >= JOYSTICK_UPPER_THRESHOLD && _direction != cv::Point(-1, 0))
         _direction = cv::Point(1, 0);
      else if (_joystick_position.y >= JOYSTICK_UPPER_THRESHOLD && _direction != cv::Point(0, 1))
         _direction = cv::Point(0, -1);
      else if (_joystick_position.x <= JOYSTICK_LOWER_THRESHOLD && _direction != cv::Point(1, 0))
         _direction = cv::Point(-1, 0);
      else if (_joystick_position.y <= JOYSTICK_LOWER_THRESHOLD && _direction != cv::Point(0, -1))
         _direction = cv::Point(0, 1);

      for (int i = 20; i < _snake_position.size(); i += 10) {
         if ((_snake_hit_box & cv::Rect(_snake_position[i].x, _snake_position[i].y, SNAKE_THICKNESS, SNAKE_THICKNESS)).area() > 0 && !_game_over) {
            _game_over = true;
            std::cout << "game over" << std::endl;
            }
         }
      if ((head.x < 0 || head.x >= _size.width || head.y < 0 || head.y >= _size.height) && !_game_over) {
         _game_over = true;
         std::cout << "game over" << std::endl;
         }
      snake_vector_logic(_apple_eaten);

      if (_button_pressed) {
         _prev_color = _color_index;
         if (_reset) {
            _color_index = 0;
            _reset = false;
            }
         else {
            _color_index = (_color_index + 1) % _colorArray.size();
            }
         _button_pressed = false;
         _color_flag = true;
         }
      std::lock_guard<std::mutex> lock(_music_mutex);
      if (_color_index == _colorArray.size() - 1 && !_musicplaying) {
         mciSendString("play myMusic repeat", NULL, 0, NULL);
         _musicplaying = true;
         _apple_spawn_rate = APPLE_SPAWN_FAST;
         _snake_speed = 20;
         _step_size = 15;
         }
      else if (_color_index != _colorArray.size() - 1 && _musicplaying) {
         mciSendString("stop myMusic", NULL, 0, NULL);
         _musicplaying = false;
         _apple_spawn_rate = APPLE_SPAWN_SPEED;
         _snake_speed = 100;
         _step_size = 10;
         }
      //////////////////////////////////////////////////////////////ChatGPT
      auto update_end = std::chrono::steady_clock::now();
      auto update_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(update_end - update_start);
      _update_time = static_cast<int>((std::chrono::duration_cast<std::chrono::microseconds>(update_end - update_start)).count());
      int sleep_duration = _snake_speed - static_cast<int>(update_elapsed.count());
      if (sleep_duration < 0)
         sleep_duration = 0;
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep_duration));
      /////////////////////////////////////////////////////////////////////
      }
   }

void CSnakeGameV2::draw() {
   while (!getExit()) {
      auto draw_start = std::chrono::steady_clock::now(); ////ChatGPT
      _frameCount++;
      setCanvas(cv::Mat::zeros(_size, CV_8UC3));
      cv::Mat& canvas = getCanvas();
      cvui::window(canvas, 10, 10, 260, 200, "Snake (" +
         std::to_string(_snake_position.front().x) + "," +
         std::to_string(_snake_position.front().y) + ")" +
         " (FPS = " + std::to_string(std::round(_FPS * 100) / 100) + ")");
      cvui::text(canvas, 20, 40, "Color: " + _colorArray[_color_index]._color);
      cvui::text(canvas, 20, 60, "Score: " + std::to_string(_score));
      cvui::text(canvas, 100, 70, "Snake Step");
      cvui::trackbar(canvas, 10, 70, 250, &_step_size, 1, MAX_STEP);
      cvui::text(canvas, 100, 125, "Snake Speed");
      cvui::trackbar(canvas, 10, 125, 250, &_snake_speed, 10, 500);
      if (cvui::button(canvas, 20, 180, "Reset")) {
         _reset = true;
         }
      if (cvui::button(canvas, 100, 180, "Exit")) {
         std::cout << "game exited" << std::endl;
         setExit(true);
         }
      if (!_game_over) {
         for (int i = 0; i < _snake_position.size(); i += 5) {
            cv::rectangle(canvas,
               cv::Rect(_snake_position[i].x, _snake_position[i].y, SNAKE_THICKNESS, SNAKE_THICKNESS),
               _colorArray[_color_index]._color_scalar,
               cv::FILLED);
            }
         for (int i = 0; i < _apple_position.size(); i++) {
            cv::circle(canvas, cv::Point(_apple_position[i].x + APPLE_RADIUS, _apple_position[i].y + APPLE_RADIUS),
               APPLE_RADIUS, _colorArray[3]._color_scalar, cv::FILLED);
            }
         }
      else {
         cv::Rect roiRect(_size.width /3, _size.height /3, overlay.cols, overlay.rows); //ChatGPT
         cv::Mat roi = canvas(roiRect); //ChatGPT
         overlay.copyTo(roi); //ChatGPT

         cv::putText(canvas, "GAME OVER", cv::Point(_size.width / 3,_size.height /5),
            cv::FONT_HERSHEY_SIMPLEX, 2.0, _colorArray[3]._color_scalar, 2);
         cv::putText(canvas, "YOUR SCORE:" + std::to_string(_score),
            cv::Point(_size.width / 4, (_size.height / 5 * 1.5)),
            cv::FONT_HERSHEY_SIMPLEX, 2.0, _colorArray[3]._color_scalar, 2);

         }
      cvui::update();
      cv::imshow(GAME_NAME, canvas);

      if (cv::waitKey(1) == 'q') {
         std::cout << "game exited" << std::endl;
         setExit(true);
         }
      if (getExit()) {
         cv::destroyAllWindows();
         break;
         }
      if (!_game_over)
      std::cout << "gpio:" << _gpio_time << "us update:" << _update_time << "us draw:" << _draw_time << "us" << std::endl;
      //////////////////////////////////////////////////////////////////////ChatGPT
      auto draw_end = std::chrono::steady_clock::now();
      _draw_time = static_cast<int>((std::chrono::duration_cast<std::chrono::microseconds>(draw_end - draw_start)).count());
      auto draw_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(draw_end - draw_start);
      int sleep_duration = (1000 / MAX_FPS) - static_cast<int>(draw_elapsed.count());
      if (sleep_duration < 0)
         sleep_duration = 0;
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep_duration));
      /////////////////////////////////////////////////////////////////////////////
      }
   }

void CSnakeGameV2::snake_vector_logic(bool apple_flag) {
   cv::Point head = _snake_position.front();
   if (apple_flag) {
      for (int i = 1; i <= SNAKE_THICKNESS; i++) {
         _snake_position.push_front(head + (_direction * i));
         }
      }
   else {
      for (int i = 1; i <= _step_size; i++) {
         _snake_position.push_front(head + (_direction * i));
         _snake_position.pop_back();
         }
      }
   }

void CSnakeGameV2::apple_spawn() {
   bool apple_insnake = true;
   cv::Rect apple;
   while (apple_insnake) {
      apple_insnake = false;
      apple = cv::Rect(rand() % _size.width, rand() % _size.height, APPLE_RADIUS * 2, APPLE_RADIUS * 2);
      for (int i = 0; i < _snake_position.size(); i += 5) {
         if ((apple & cv::Rect(_snake_position[i].x, _snake_position[i].y, SNAKE_THICKNESS, SNAKE_THICKNESS)).area() > 0) {
            apple_insnake = true;
            break;
            }
         }
      }
   _apple_position.push_back(apple);
   }

void CSnakeGameV2::reset() {
   std::cout << "game reset" << std::endl;
   _apple_eaten = false;
   _game_over = false;
   _snake_length = STARTING_LENGTH;
   _score = 0;
   _direction = cv::Point(0, -1);
   _step_size = 10;
   _snake_position.clear();
   _apple_position.clear();
   cv::Point start_position(_size.width / 2, _size.height / 2);
   for (int i = 0; i < STARTING_LENGTH; ++i) {
      _snake_position.push_back(cv::Point(start_position.x, start_position.y + i));
      }
   _button_pressed = true;
   _snake_speed = 100;
   overlay = cv::imread("C:\\Users\\manta\\Downloads\\ELEX4618\\ELEX4618-Template-master\\Snake.jpg", cv::IMREAD_COLOR);
   mciSendString("open \"C:\\Users\\manta\\Downloads\\ELEX4618\\ELEX4618-Template-master\\Music.mp3\" type mpegvideo alias myMusic", NULL, 0, NULL);
   }
