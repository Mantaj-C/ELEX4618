#include "stdafx.h"
#include "cvui.h"
#include "CSnakeGameV2.h"
#include <windows.h>
#include <mmsystem.h>
#include <vector>

#define GAME_NAME "Mantaj Chauhan Snake Game"
#define SNAKE_THICKNESS 10
#define SCORE_INCREMENT 1
#define APPLE_RADIUS 10
#define STARTING_LENGTH 100
#define MAX_STEP 20
#define JOYSTICK_X 2
#define JOYSTICK_Y 26
#define SWITCH_1 33
#define SWITCH_2 32
#define RGBLED_RED_PIN 39
#define JOYSTICK_UPPER_THRESHOLD 80.0
#define JOYSTICK_LOWER_THRESHOLD 20.0
#define RGBLED_RED_PIN 39
#define RGBLED_GREEN_PIN 38
#define RGBLED_BLUE_PIN 37
#define BUZZER 40

enum { DIGITAL = 0, ANALOG, SERVO };

CSnakeGameV2::CSnakeGameV2(cv::Size size) : _size(size), _reset(true), _color_flag(true),
_color_index(0), _joystick_position(0, 0), _button_pressed(false), _musicplaying(false), 
   _step_size(10), _snake_speed(100){
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
   mciSendString("open \"C:\\Users\\manta\\Downloads\\ELEX4618\\ELEX4618-Template-master\\Music.mp3\" type mpegvideo alias myMusic", NULL, 0, NULL); //ChatGPT
   setCanvas(cv::Mat::zeros(size, CV_8UC3));
   cvui::init(GAME_NAME);
   }

CSnakeGameV2::~CSnakeGameV2() {
   getControl().set_data(DIGITAL, RGBLED_BLUE_PIN, 0);
   getControl().set_data(DIGITAL, RGBLED_RED_PIN, 0);
   getControl().set_data(DIGITAL, RGBLED_GREEN_PIN, 0);
   mciSendString("stop myMusic", NULL, 0, NULL); //ChatGPT
   mciSendString("close myMusic", NULL, 0, NULL); //ChatGPT

   }

void CSnakeGameV2::gpio() {
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
   }

void CSnakeGameV2::update() {
   if (_reset) {
      reset();
      }

   double currentTime = cv::getTickCount() / cv::getTickFrequency();

   if (currentTime - _lastTime_frame >= 1.0) {
      _FPS = _frameCount / (currentTime - _lastTime_frame);
      _frameCount = 0;
      _lastTime_frame = currentTime;
      }

   if (currentTime - _lastTime_apple >= 5.0 && !_apple_inplay) {
      apple_spawn();
      }

   _snake_hit_box = cv::Rect(_snake_position.front().x, _snake_position.front().y, SNAKE_THICKNESS, SNAKE_THICKNESS);
   _apple_hit_box = cv::Rect(_apple_position.x - APPLE_RADIUS, _apple_position.y - APPLE_RADIUS, APPLE_RADIUS*2, APPLE_RADIUS*2);

   if ((_snake_hit_box & _apple_hit_box).area() > 0 && _apple_inplay) {
      _apple_eaten = true;
      }

   if (_apple_eaten) {
      _lastTime_apple = currentTime;
      _score = _score + SCORE_INCREMENT;
      snake_vector_logic(_apple_eaten);
      _apple_eaten = false;
      _apple_inplay = false;
      }

   cv::Point head = _snake_position.front();

   _colorArray[3]._color_scalar = cv::Scalar(rand() % 256, rand() * rand() % 256, rand() * rand() * rand() % 256);

   //right
   if (_joystick_position.x >= JOYSTICK_UPPER_THRESHOLD && _direction != cv::Point(-1, 0)) {
      _direction = cv::Point(1, 0);
      }
   
   //up
   else if (_joystick_position.y >= JOYSTICK_UPPER_THRESHOLD && _direction != cv::Point(0, 1)) {
      _direction = cv::Point(0, -1);
      }

   //left
   else if (_joystick_position.x <= JOYSTICK_LOWER_THRESHOLD && _direction != cv::Point(1, 0)) {
      _direction = cv::Point(-1, 0);
      }

   //down
   else if (_joystick_position.y <= JOYSTICK_LOWER_THRESHOLD && _direction != cv::Point(0, -1)) {
      _direction = cv::Point(0, 1);
      }

   snake_vector_logic(_apple_eaten);

   for (int i = 2; i * SNAKE_THICKNESS < _snake_length; i = i + 2) {
      if ((_snake_hit_box & cv::Rect(_snake_position[i* SNAKE_THICKNESS].x, _snake_position[i* SNAKE_THICKNESS].y, SNAKE_THICKNESS, SNAKE_THICKNESS)).area() > 0) {
         game_over();
         }
      }
   if (head.x < 0 || head.x >= _size.width || head.y < 0 || head.y >= _size.height) {
      game_over();
      }

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
   if (_color_index == _colorArray.size() - 1 && !_musicplaying) {
      mciSendString("play myMusic repeat", NULL, 0, NULL); //ChatGPT
      _musicplaying = true;
      }
   else if (_color_index != _colorArray.size() - 1 && _musicplaying) {
      mciSendString("stop myMusic", NULL, 0, NULL); //ChatGPT
      _musicplaying = false;
      }
   }

void CSnakeGameV2::draw() {
   _frameCount++;
   setCanvas(cv::Mat::zeros(_size, CV_8UC3));
   cv::Mat& canvas = getCanvas();
   ;
   cvui::window(canvas, 10, 10, 260, 200, "Snake (" + std::to_string(_snake_position.front().x)
      + "," + std::to_string(_snake_position.front().y) + ")" + " (FPS = " + std::to_string(std::round(_FPS * 100) / 100) + ")");
   cvui::text(canvas, 20, 40, "Color: " + _colorArray[_color_index]._color);
   cvui::text(canvas, 20, 60, "Score: " + std::to_string(_score));
   cvui::text(canvas, 100, 70, "Snake Step");
   cvui::trackbar(canvas, 10, 70, 250, &_step_size, 1, MAX_STEP);
   cvui::text(canvas, 100, 125, "Snake Speed");
   cvui::trackbar(canvas, 10, 125, 250, &_snake_speed, 10, 500);
   if (cvui::button(canvas, 20, 180, "Reset")) {
      _reset = true;
      }

   if (cvui::button(getCanvas(), 100, 180, "Exit")) {
      std::cout << "game exited" << std::endl;
      setExit(true);
      }
   /////////////////////////////////////////////////ChatGPT
   for (const auto& segment : _snake_position) {
      cv::rectangle(canvas,
         cv::Rect(segment.x, segment.y, SNAKE_THICKNESS, SNAKE_THICKNESS),
         _colorArray[_color_index]._color_scalar,
         cv::FILLED);
      }
   ///////////////////////////////////////////////////
   if (_apple_inplay) {
      cv::circle(canvas, _apple_position, APPLE_RADIUS, _colorArray[3]._color_scalar, cv::FILLED);
      }
   cvui::update();
   cv::imshow(GAME_NAME, canvas);
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
      _snake_position.push_front(head + (_direction*i));
      _snake_position.pop_back();
      }
   }
}

void CSnakeGameV2::apple_spawn() {
   bool apple_insnake = false;
   do {
      apple_insnake = false;
      _apple_position.x = rand() % _size.width;
      _apple_position.y = rand() % _size.height;
      for (int i = 0; i < _snake_position.size(); i = i + 5) {
         if (_apple_position == _snake_position[i]) {
            apple_insnake = true;
            break;
            }
         }
      } while (apple_insnake);
         _apple_inplay = true;
   }

void CSnakeGameV2::reset() {
      std::cout << "game reset" << std::endl;
      _apple_eaten = false;
      _apple_inplay = false;
      _snake_length = STARTING_LENGTH;
      _score = 0;
      _direction = cv::Point(0, -1);
      _step_size = 10;
      _snake_position.clear();
      cv::Point start_position(_size.width / 2, _size.height / 2);
      for (int i = 0; i < STARTING_LENGTH; ++i) {
         _snake_position.push_back(cv::Point(start_position.x, start_position.y + i));
         }
      _button_pressed = true;
      _snake_speed = 100;
   }

void CSnakeGameV2::game_over() {
   std::cout << "game over" << std::endl;
   }