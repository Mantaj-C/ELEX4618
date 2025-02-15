#define CVUI_IMPLEMENTATION
#include "CAsteroidGame.h"

#define GAME_NAME "Mantaj Chauhan Asteroid Game"
#define SWITCH_1 33
#define SWITCH_2 32
#define SWITCH_3 5
#define JOYSTICK_X 2
#define JOYSTICK_UPPER_THRESHOLD 80.0
#define JOYSTICK_LOWER_THRESHOLD 20.0
#define MAX_FPS 30
#define UPDATE_SPEED 20.0
#define SCORE_INCREMENT 10
#define ASTEROID_RATE 0.5

enum { TILT_LEFT = -1, TILT_RIGHT = 1 };
enum { DIGITAL = 0, ANALOG, SERVO };
enum { TIP = 0, LEFT_CORNER, NOTCH, RIGHT_CORNER };

CAsteroidGame::CAsteroidGame(cv::Size size) {
   _reset = false;
   _game_over = false;
   _button_pressed = false;
   _FPS = 0.0f;
   _framecount = 0;
   _score = 0;
   _missile_fire = false;
   _thrusters = false;
   _hyperspace = false;
   _lastTime_frame = cv::getTickCount() / cv::getTickFrequency();
   _lastTime_asteroid = cv::getTickCount() / cv::getTickFrequency();
   _size = size;
   _ship = CShip(_size);
   setCanvas(cv::Mat::zeros(size, CV_8UC3));
   cvui::init(GAME_NAME);
   }

CAsteroidGame::~CAsteroidGame() {
   std::cout << "game exited" << std::endl;
   cv::destroyAllWindows();
   }

void CAsteroidGame::gpio() {
   while (!getExit()) {
      auto gpio_start = std::chrono::steady_clock::now();//ChatGPT
      int button1_state = 0;
      int button2_state = 0;
      int button3_state = 0;
      _joystick_position = getControl().get_analog(ANALOG, JOYSTICK_X);
      if (getControl().get_button(SWITCH_2, button1_state)) {
         _missile_fire = true;
         }
      else if (getControl().get_button(SWITCH_1, button2_state)) {
         _thrusters = true;
         }
      else if (getControl().get_button(SWITCH_3, button2_state)) {
         _hyperspace = true;
         }
      ////////////////////////////////////////////////////////////////////ChatGPT
      auto gpio_end = std::chrono::steady_clock::now();
      _gpio_time = static_cast<int>((std::chrono::duration_cast<std::chrono::microseconds>(gpio_end - gpio_start)).count());
      ////////////////////////////////////////////////////////////////////ChatGPT
      }
   }

void CAsteroidGame::update() {
   while (!getExit()) {
      if (_reset)
         reset();

      auto update_start = std::chrono::steady_clock::now(); //ChatGPT
      double currentTime = cv::getTickCount() / cv::getTickFrequency();

      if (currentTime - _lastTime_asteroid >= ASTEROID_RATE) {
         CAsteroid asteroid(_size);
         _asteroids.push_back(asteroid);
         std::cout << "Asteroid Spawned" << std::endl;
         }

      if (_missile_fire) {
         CMissile missile(_ship.get_shape(TIP) + _ship.get_pos, _ship.get_shape(NOTCH) + _ship.get_pos);
         _missiles.push_back(missile);
         std::cout << "Missles Fired" << std::endl;
         _missile_fire = false;
         }

      if (_hyperspace) {
         std::cout << "Hyperspace Was Activated" << std::endl;
         _hyperspace = false;
         }
      if (_asteroids.size() > 0) { //missing asteroid with wall
         for (int i = 0; i <= _asteroids.size() - 1; ++i) {
            if (_missiles.size() > 0) {
               for (int j = 0; j <= _asteroids.size() - 1; ++j) {
                  if (_asteroids[i].collide(_missiles[j])) {
                     _asteroids[i].hit();
                     _missiles[j].hit();
                     }
                  if (_missiles[j].collide_wall())
                     _missiles[j].hit();
                  }
               }
            if (_ship.collide(_asteroids[i])) {
               _ship.hit();
               _asteroids[i].hit();
               }
            }
         }
      if (cv::waitKey(1) == 'q')
         setExit(true);
      //////////////////////////////////////////////////////////////ChatGPT
      auto update_end = std::chrono::steady_clock::now();
      auto update_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(update_end - update_start);
      _update_time = static_cast<int>((std::chrono::duration_cast<std::chrono::microseconds>(update_end - update_start)).count());
      int sleep_duration = UPDATE_SPEED - static_cast<int>(update_elapsed.count());
      if (sleep_duration < 0)
         sleep_duration = 0;
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep_duration));
      /////////////////////////////////////////////////////////////////////
      }
   }

void CAsteroidGame::draw() {
   while (!getExit()) {
      auto draw_start = std::chrono::steady_clock::now(); ////ChatGPT
      _frameCount++;
      setCanvas(cv::Mat::zeros(_size, CV_8UC3));
      cv::Mat& canvas = getCanvas();
      cvui::window(canvas, 10, 10, 260, 230, "Ship (" +
         std::to_string(_ship.get_pos.x) + "," +
         std::to_string(_ship.get_pos.y) + ")" +
         " (FPS = " + std::to_string(std::round(_FPS * 100) / 100) + ")");
      cvui::text(canvas, 20, 60, "Score: " + std::to_string(_score));
      if (cvui::button(canvas, 20, 210, "Reset")) {
         _reset = true;
         }
      if (cvui::button(canvas, 100, 210, "Exit")) {
         setExit(true);
         }
      _ship.draw(canvas);
      for (int i = 0; i <= _asteroids.size() - 1; ++i)
         _asteroids[i].draw();
      for (int i = 0; i <= _missiles.size() - 1; ++i)
         _missiles[i].draw();
      cvui::update();
      cv::imshow(GAME_NAME, canvas);
      //std::cout << "gpio:" << _gpio_time << "us update:" << _update_time << "us draw:" << _draw_time << "us" << std::endl;
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

void CAsteroidGame::reset() {
   _ship = CShip(_size);
   _score = 0;
   _button_pressed = false;
   _missile_fire = false;
   _thrusters = false;
   _hyperspace = false;
   _lastTime_asteroid = cv::getTickCount() / cv::getTickFrequency();
   _missiles.clear();
   _asteroids.clear();
   std::cout << "game reset" << std::endl;
   _reset = false;
   }