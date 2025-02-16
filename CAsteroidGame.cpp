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
#define CONTROLLER_LOWER_THRESHOLD -80
#define MAX_FPS 60
#define UPDATE_SPEED 50.0
#define SCORE_INCREMENT 10
#define ASTEROID_RATE 1.5
#define INVADER_RATE 10
#define INVADER_FIRE_RATE 1.5

enum { TILT_LEFT = -1, TILT_RIGHT = 1 };
enum { DIGITAL = 0, ANALOG, SERVO };
enum { TIP = 0, LEFT_CORNER, NOTCH, RIGHT_CORNER };

CAsteroidGame::CAsteroidGame(cv::Size size) : _ship(size), _size(size),
   _ship_lives_image(size), _controller(false){
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
   _lastTime_invader = cv::getTickCount() / cv::getTickFrequency();
   char userin;
   std::cout << "(1) MicroController" << std::endl;
   std::cout << "(2) Playstation Controller" << std::endl;
   std::cout << "CMD>";
   std::cin >> userin;
   if (userin == '2')
      _controller = true;
   else
      _controller = false;
   _button_states.push_back(false);
   _button_states.push_back(false);
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
      if (_controller) {
         sf::Joystick::update();
         if (sf::Joystick::isButtonPressed(0, 4))
            _joystick_position = 0;
         else if (sf::Joystick::isButtonPressed(0, 5))
            _joystick_position = 100;
         else 
            _joystick_position = 50;
         bool currentState1 = sf::Joystick::isButtonPressed(0, 1);
         bool currentState2 = sf::Joystick::isButtonPressed(0, 3);
         if (sf::Joystick::isButtonPressed(0, 2))
            _thrusters = true;
         if (currentState1 && !_button_states[0])
            _missile_fire = true;
         if (currentState2 && !_button_states[1])
            _hyperspace = true;
         _button_states[0] = currentState1;
         _button_states[1] = currentState2;
         }
      else {
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

      if (currentTime - _lastTime_invader >= INVADER_RATE) {
         CInvader invader(_size);
         _invaders.push_back(invader);
         std::cout << "Invader Spawned" << std::endl;
         _lastTime_invader = currentTime;
         }

      if ((currentTime - _lastTime_invader_fire >= INVADER_FIRE_RATE) && _invaders.size() >= 1) {
         std::random_device rd; //ChatGPT
         std::mt19937 engine(rd()); //ChaptGPT
         std::uniform_int_distribution<int> distribution2(0, _invaders.size() - 1);
         int invader_select = distribution2(engine);
         CMissile missile(_ship.get_pos(), _invaders[invader_select].get_pos(),3.7);
         _invaders_missiles.push_back(missile);
         std::cout << "Invader Missiles Fired" << std::endl;
         _lastTime_invader_fire = currentTime;
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
////////////////////////////////////////////////////////////
// Process collisions for asteroids
      int i = 0;
      while (i < _asteroids.size()) {
         // Move this asteroid.
         _asteroids[i].move();
         if (_asteroids[i].collide_wall(_size))
            _asteroids[i].hit();

         // Check collision with the ship.
         if (_ship.collide(_asteroids[i])) {
            _ship.hit();
            if (_ship.get_lives() <= 0)
               _game_over = true;
            _asteroids[i].hit();
            }

         // Process collisions with missiles for this asteroid.
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

            // If missile is dead, remove it and do not increment j.
            if (_missiles[j].get_lives() <= 0) {
               _missiles.erase(_missiles.begin() + j);
               continue;
               }
            j++;
            }

         // If this asteroid is dead, erase it and do not increment i.
         if (_asteroids[i].get_lives() <= 0) {
            _asteroids.erase(_asteroids.begin() + i);
            continue;
            }
         i++;
         }

      // Process collisions for invaders
      int k = 0;
      while (k < _invaders.size()) {
         // Move this invader.
         _invaders[k].move();
         // Optionally, you might spawn missiles from invaders here.
         // For example, create a missile from the invader toward the sh

         if (_invaders[k].collide_wall(_size))
            _invaders[k].hit();

         // Check collision with the ship.
         if (_ship.collide(_invaders[k])) {
            _ship.hit();
            if (_ship.get_lives() <= 0)
               _game_over = true;
            _invaders[k].hit();
            }

         // Process collisions with missiles for this invader.
         int j = 0;
         while (j < _missiles.size()) {
            _missiles[j].move();
            if (_missiles[j].collide_wall(_size))
               _missiles[j].hit();

            if (_invaders[k].collide(_missiles[j])) {
               _invaders[k].hit();
               _missiles[j].hit();
               _score += SCORE_INCREMENT;
               }

            if (_missiles[j].collide_wall(_size))
               _missiles[j].hit();

            if (_missiles[j].get_lives() <= 0) {
               _missiles.erase(_missiles.begin() + j);
               continue;
               }
            j++;
            }

         if (_invaders[k].get_lives() <= 0) {
            _invaders.erase(_invaders.begin() + k);
            continue;
            }
         k++;
         }

      // Process collisions for invader missiles
      int m = 0;
      while (m < _invaders_missiles.size()) {
         // Move the missile.
         _invaders_missiles[m].move();

         // Check if it hits the wall.
         if (_invaders_missiles[m].collide_wall(_size))
            _invaders_missiles[m].hit();

         // Check collision with the ship.
         if (_ship.collide(_invaders_missiles[m])) {
            _ship.hit();
            if (_ship.get_lives() <= 0)
               _game_over = true;
            _invaders_missiles[m].hit();
            }

         // If the missile is dead, remove it and do not increment m.
         if (_invaders_missiles[m].get_lives() <= 0) {
            _invaders_missiles.erase(_invaders_missiles.begin() + m);
            continue;
            }
         m++;
         }

      ///////////////////////////////////////////////////////////////
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
      for (int i = 0; i < _invaders.size(); i++)
         _invaders[i].draw(canvas);
      for (int i = 0; i < _invaders_missiles.size(); i++)
         _invaders_missiles[i].draw(canvas);
         }
      

      cvui::update();
      cv::imshow(GAME_NAME, canvas);
      if (cv::waitKey(1) == 'q')
         setExit(true);
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
   _lastTime_invader = cv::getTickCount() / cv::getTickFrequency();
   _missiles.clear();
   _asteroids.clear();
   _invaders.clear();
   _invaders_missiles.clear();
   std::cout << "game reset" << std::endl;
   _game_over = false;
   _reset = false;
   }