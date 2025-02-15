#include "stdafx.h"
#include "CAsteroidGame.h"
#include "cvui.h"

#define GAME_NAME "Mantaj Chauhan Asteroid Game"
#define SWITCH_1 33
#define SWITCH_2 32
#define SWITCH_3 31
#define JOYSTICK_X 2
#define JOYSTICK_UPPER_THRESHOLD 80.0
#define JOYSTICK_LOWER_THRESHOLD 20.0
#define MAX_FPS 60
#define UPDATE_SPEED 50.0
#define SCORE_INCREMENT 10
#define ASTEROID_RATE 3

enum { TILT_LEFT = -1, TILT_RIGHT = 1 };
enum { DIGITAL = 0, ANALOG, SERVO };
enum { TIP = 0, LEFT_CORNER, NOTCH, RIGHT_CORNER };

CAsteroidGame::CAsteroidGame(cv::Size size) : _ship(size), _size(size),
   _ship_lives_image(size){
   _reset = false;
   _game_over = false;
   _button_pressed = false;
   _FPS = 0.0f;
   _frameCount = 0;
   _score = 0;
   _missile_fire = false;
   _thrusters = false;
   _hyperspace = false;
   _lastTime_frame = cv::getTickCount() / cv::getTickFrequency();
   _lastTime_asteroid = cv::getTickCount() / cv::getTickFrequency();
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
      if (getControl().get_button(SWITCH_2, button2_state)) {
         _missile_fire = true;
         }
      else if (getControl().get_button_hold(SWITCH_1, button1_state)) {
         _thrusters = true;
         }
      else if (getControl().get_button(SWITCH_3, button3_state)) {
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

      if (currentTime - _lastTime_frame >= 1.0) {
         _FPS = _frameCount / (currentTime - _lastTime_frame);
         _frameCount = 0;
         _lastTime_frame = currentTime;
         }

      if (currentTime - _lastTime_asteroid >= ASTEROID_RATE) {
         CAsteroid asteroid(_size);
         _asteroids.push_back(asteroid);
         std::cout << "Asteroid Spawned" << std::endl;
         _lastTime_asteroid = currentTime;
         }

      _ship.move(_thrusters);
      _thrusters = false;

      if (_joystick_position >= JOYSTICK_UPPER_THRESHOLD)
         _ship.tilt(TILT_RIGHT);
      if (_joystick_position <= JOYSTICK_LOWER_THRESHOLD)
         _ship.tilt(TILT_LEFT);

      if (_hyperspace) {
         std::cout << "Hyperspace Was Activated" << std::endl;
         std::random_device rd; //ChatGPT
         std::mt19937 engine(rd()); //ChaptGPT
         std::uniform_int_distribution<int> distribution1(0, _size.width); //ChatGPT
         std::uniform_int_distribution<int> distribution2(0, _size.height); //ChatGPT
         _ship.set_pos(cv::Point2f(distribution1(engine), distribution2(engine)));
         _hyperspace = false;
         }

      ////////////////////////////////////////////////////////////ChatGPT
      // Process collisions
      int i = 0;
      while (i < _asteroids.size()) {
         // Move the asteroid.
         _asteroids[i].move();
         if (_asteroids[i].collide_wall(_size))
            _asteroids[i].hit();

         if (_ship.collide(_asteroids[i])) {
            _ship.hit();
            if (_ship.get_lives() <= 0)
               _game_over = true;
            _asteroids[i].hit();
            }

         // Process collisions with missiles.
         int j = 0;
         while (j < _missiles.size()) {
            _missiles[j].move();
            if (_missiles[j].collide_wall(_size))
               _missiles[j].hit();

            if (_asteroids[i].collide(_missiles[j])) {
               _asteroids[i].hit();
               _missiles[j].hit();
               _score += SCORE_INCREMENT;
               }

            j++;
            }
         i++;
         }

      // Remove dead asteroids
      _asteroids.erase(std::remove_if(_asteroids.begin(), _asteroids.end(),
         [](const CAsteroid& ast) { return ast.get_lives() <= 0; }),
         _asteroids.end());

      // Remove dead missiles
      _missiles.erase(std::remove_if(_missiles.begin(), _missiles.end(),
         [](const CMissile& mis) { return mis.get_lives() <= 0; }),
         _missiles.end());
      //////////////////////////////////////////////////////////////

      if (_missile_fire) {
         CMissile missile(_ship.get_shape(TIP) + _ship.get_pos(), _ship.get_shape(NOTCH) + _ship.get_pos());
         _missiles.push_back(missile);
     
         std::cout << "Missles Fired" << std::endl;
         _missile_fire = false;
         }

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
      cv::Point2f lives_display_starting_pos = cv::Point2f(200, 50);
      setCanvas(cv::Mat::zeros(_size, CV_8UC3));
      cv::Mat& canvas = getCanvas();
      cvui::window(canvas, 10, 10, 150, 60,
         "FPS = " + std::to_string(std::round(_FPS * 100) / 100));
      if (cvui::button(canvas, 20, 35, "Reset")) {
         _reset = true;
         }
      if (cvui::button(canvas, 100, 35, "Exit")) {
         setExit(true);
         }
      if (_game_over) {
         cv::putText(canvas, "Your Score: " + std::to_string(_score), cv::Point2f(200, 200), cv::FONT_HERSHEY_SIMPLEX,
            3.0, cv::Scalar(255, 0, 0), 2, cv::LINE_AA);
         cv::putText(canvas, "GAME OVER", cv::Point2f(200, 300), cv::FONT_HERSHEY_SIMPLEX,
            3.0, cv::Scalar(255, 0, 0), 2, cv::LINE_AA);
         }
      else {
      for (int i = 0; i < _ship.get_lives(); i++) {
         _ship_lives_image.draw(canvas, lives_display_starting_pos + (i * cv::Point2f(50, 0)));
         }
      cv::putText(canvas, "Score: " + std::to_string(_score), cv::Point2f(800, 50), cv::FONT_HERSHEY_SIMPLEX,
         1.0, cv::Scalar(255, 0, 0), 2, cv::LINE_AA);

      _ship.draw(canvas);
      for (int i = 0; i <_asteroids.size(); i++)
         _asteroids[i].draw(canvas);
      for (int i = 0; i < _missiles.size(); i++)
         _missiles[i].draw(canvas);
         }

      cvui::update();
      cv::imshow(GAME_NAME, canvas);
      if (cv::waitKey(1) == 'q')
         setExit(true);
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
   _game_over = false;
   _reset = false;
   }