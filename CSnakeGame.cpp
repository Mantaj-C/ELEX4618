#include "CSnakeGame.h"

#define GAME_NAME "Mantaj Chauhan Snake Game"
#define UNIT_STEP 10
#define STARTING_LENGTH 200
#define JOYSTICK_X 2
#define JOYSTICK_Y 26
#define SWITCH_1 31
#define SWITCH_2 32
#define RGBLED_RED_PIN 39
#define JOYSTICK_UPPER_THRESHOLD 90.0
#define JOYSTICK_LOWER_THRESHOLD 10.0
#define RGBLED_RED_PIN 39
#define RGBLED_GREEN_PIN 38
#define RGBLED_BLUE_PIN 37

enum { DIGITAL = 0, ANALOG, SERVO };

CSnakeGame::CSnakeGame(cv::Size size) : _size(size), _reset(false), _color_flag(true),
   _color_index(0), _joystick_position(0,0), _button_pressed(false){
   _colorArray.push_back({ "Red", cv::Scalar(0,0,255), RGBLED_RED_PIN });
   _colorArray.push_back({ "Green", cv::Scalar(0,255,0), RGBLED_GREEN_PIN });
   _colorArray.push_back({ "Blue", cv::Scalar(255,0,0), RGBLED_BLUE_PIN });
   _colorArray.push_back({ "Rainbow", cv::Scalar(rand() % 256,rand() % 256,rand() % 256), RGBLED_BLUE_PIN });
   _prev_direction = cv::Point(0, UNIT_STEP);
   _prev_color = _colorArray.size() - 1;

   cv::Point start_position(_size.width / 2, _size.height / 2);
   for (int i = 0; i < STARTING_LENGTH / UNIT_STEP; ++i) {
      _snake_position.push_back(cv::Point(start_position.x, start_position.y + i * UNIT_STEP));
      }

   setCanvas(cv::Mat::zeros(size, CV_8UC3));
   cvui::init(GAME_NAME);   
   }

CSnakeGame::~CSnakeGame() {
   getControl().set_data(DIGITAL, RGBLED_BLUE_PIN, 0);
   getControl().set_data(DIGITAL, RGBLED_RED_PIN, 0);
   getControl().set_data(DIGITAL, RGBLED_GREEN_PIN, 0);

   }

void CSnakeGame::gpio() {
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

void CSnakeGame::update() {
   _colorArray[3]._color_scalar = cv::Scalar(rand() % 256, rand()* rand() % 256, rand()* rand()* rand() % 256);
   
   if (_joystick_position.x >= JOYSTICK_UPPER_THRESHOLD && _prev_direction != cv::Point(-UNIT_STEP, 0) 
      && _snake_position.front().x != _size.width - UNIT_STEP) {
      _snake_position.push_front(_snake_position.front() + cv::Point(UNIT_STEP, 0));
      _prev_direction = cv::Point(UNIT_STEP, 0);
      _snake_position.pop_back();
      }

   else if (_joystick_position.y >= JOYSTICK_UPPER_THRESHOLD && _prev_direction != cv::Point(0, UNIT_STEP) 
      && _snake_position.front().y != 0) {
      _snake_position.push_front(_snake_position.front() + cv::Point(0, -UNIT_STEP));
      _prev_direction = cv::Point(0, -UNIT_STEP);
      _snake_position.pop_back();
      }

   else if (_joystick_position.x <= JOYSTICK_LOWER_THRESHOLD && _prev_direction != cv::Point(UNIT_STEP, 0) 
      && _snake_position.front().x != 0) {
      _snake_position.push_front(_snake_position.front() + cv::Point(-UNIT_STEP, 0));
      _prev_direction = cv::Point(-UNIT_STEP, 0);
      _snake_position.pop_back();
      }

   else if (_joystick_position.y <= JOYSTICK_LOWER_THRESHOLD && _prev_direction != cv::Point(0, -UNIT_STEP) 
      && _snake_position.front().y != _size.height - UNIT_STEP) {
      _snake_position.push_front(_snake_position.front() + cv::Point(0, UNIT_STEP));
      _prev_direction = cv::Point(0, UNIT_STEP);
      _snake_position.pop_back();
      }

   if (_button_pressed) {
      _prev_color = _color_index;
      _color_index = (_color_index + 1) % _colorArray.size(); 
      _button_pressed = false;
      _color_flag = true;
      }
   
   if (_reset) {
      std::cout << "game reset" << std::endl;
      _prev_direction = cv::Point(0, UNIT_STEP);
      _prev_color = _color_index;
      _snake_position.clear();
      cv::Point start_position(_size.width / 2, _size.height / 2);
      for (int i = 0; i < STARTING_LENGTH / UNIT_STEP; ++i) {
         _snake_position.push_back(cv::Point(start_position.x, start_position.y + i * UNIT_STEP));
         }
      _reset = false;
      _color_index = 0;
      _color_flag = true;
      }
   }

void CSnakeGame::draw() {
   setCanvas(cv::Mat::zeros(_size, CV_8UC3));
   cv::Mat& canvas = getCanvas();
   ;
   cvui::window(canvas, 10, 10, 150, 90, "Snake (" + std::to_string(_snake_position.front().x) 
      + "," + std::to_string(_snake_position.front().y) + ")");
   cvui::text(canvas, 20, 40, "Color: " + _colorArray[_color_index]._color);
   if (cvui::button(canvas, 20, 70, "Reset")) {
      _reset = true;
      }

   if (cvui::button(getCanvas(), 80, 70, "Exit")) {
      std::cout << "game exited" << std::endl;
      setExit(true);
      }
   /////////////////////////////////////////////////ChatGPT
   for (const auto& segment : _snake_position) {
      cv::rectangle(canvas,
         cv::Rect(segment.x, segment.y, UNIT_STEP, UNIT_STEP),
         _colorArray[_color_index]._color_scalar,
         cv::FILLED);
      }
   ///////////////////////////////////////////////////
   cvui::update();
   cv::imshow(GAME_NAME, canvas);
   }