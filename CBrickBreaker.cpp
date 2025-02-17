#include "stdafx.h"
#include "CBrickBreaker.h"
#include "cvui.h"
#include <algorithm>

#define GAME_NAME "Mantaj Chauhan Brick Breaker Game"
#define SWITCH_1 32
#define JOYSTICK_X 2
#define JOYSTICK_UPPER_THRESHOLD 80.0
#define JOYSTICK_LOWER_THRESHOLD 20.0
#define MAX_FPS 60
#define UPDATE_SPEED 50.0
#define SCORE_INCREMENT 1
#define BALL_INITIAL_VELOCITY 15
#define BRICK_WIDTH 50
#define BRICK_HEIGHT 30

enum { DIGITAL = 0, ANALOG, SERVO };
enum { LEFT = -1, NO, RIGHT };

CBrickBreaker::CBrickBreaker(cv::Size size) : _ball(size), _size(size), _controller(false), 
_paddle(size), _button_state(false), level(1) {
   _reset = false;
   _game_over = false;
   _FPS = 0.0f;
   _frameCount = 0;
   _score = 0;
   _lastTime_frame = cv::getTickCount() / cv::getTickFrequency();
   char userin;
   std::cout << "(1) MicroController" << std::endl;
   std::cout << "(2) Playstation Controller" << std::endl;
   std::cout << "CMD>";
   std::cin >> userin;
   if (userin == '2')
      _controller = true;
   else
      _controller = false;
   _rainbow = RainbowColors::getRainbowColors();
   _rainbow_select = 0;
   level_create(level);
   setCanvas(cv::Mat::zeros(size, CV_8UC3));
   cvui::init(GAME_NAME);
   }

CBrickBreaker::~CBrickBreaker() {
   std::cout << "game exited" << std::endl;
   cv::destroyAllWindows();
   }

void CBrickBreaker::gpio() {
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
         _button_state = sf::Joystick::isButtonPressed(0, 2);
         }
      else {
         int button1_state = 0;
         _joystick_position = getControl().get_analog(ANALOG, JOYSTICK_X);
         if (getControl().get_button(SWITCH_1, button1_state))
            _button_state = true;
         }
      ////////////////////////////////////////////////////////////////////ChatGPT
      auto gpio_end = std::chrono::steady_clock::now();
      _gpio_time = static_cast<int>((std::chrono::duration_cast<std::chrono::microseconds>(gpio_end - gpio_start)).count());
      ////////////////////////////////////////////////////////////////////ChatGPT
      }
   }

void CBrickBreaker::update() {
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

      _ball.move();

      if (_button_state && (_ball.get_vel() == cv::Point2f(0, 0))) {
         _ball.set_vel(cv::Point2f(0, BALL_INITIAL_VELOCITY));
         _button_state = false;
         }

      if (_ball.get_vel() == cv::Point2f(0, 0))
      _ball.set_pos(cv::Point2f(_paddle.get_pos().x, _paddle.get_pos().y - 
         (PADDLE_HEIGHT / 2) - _ball.get_rad() - 2));

      if (_joystick_position >= JOYSTICK_UPPER_THRESHOLD)
         _paddle.move(RIGHT);
      else if (_joystick_position <= JOYSTICK_LOWER_THRESHOLD)
         _paddle.move(LEFT);
      else
         _paddle.move(NO);

      if (_ball.get_vel() != cv::Point2f(0, 0)) {


         _ball.collide_paddle(_paddle);
         if (_ball.collide_wall(_size,_paddle.get_pos()))
            _ball.hit();
         for (int i = 0; i < _bricks.size(); i++) {
            if (_ball.collide_brick(_bricks[i])) {
               _score += SCORE_INCREMENT;
               _bricks[i].hit();
               }
            }
         }

      _bricks.erase(
         std::remove_if(_bricks.begin(), _bricks.end(),
            [](const CBricks& brick) { return brick.get_lives() <= 0; }),
         _bricks.end()
      );

      if (_ball.get_lives() <= 0)
         _game_over = true;

      if (_bricks.size() <= 0) {
         level++;
         level_create(level);
         _ball = CBall(_size);
         _paddle = CPaddle(_size);
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

void CBrickBreaker::draw() {
   while (!getExit()) {
      auto draw_start = std::chrono::steady_clock::now(); ////ChatGPT
      _frameCount++;
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
         _ball.draw(canvas);
         _paddle.draw(canvas,cv::Scalar(0,0,255));
         _rainbow_select = 0;
         for (int i = 0; i < _bricks.size(); i++) {
            if (_rainbow_select >= 99)
               _rainbow_select = 0;
            _bricks[i].draw(canvas,_rainbow[_rainbow_select]);
            _rainbow_select += 3;
            }
         cv::putText(canvas, "Score:" + std::to_string(_score), cv::Point2f(800, 50), cv::FONT_HERSHEY_SIMPLEX,
            1.0, cv::Scalar(255, 0, 0), 2, cv::LINE_AA);
         for (int i = 0; i < _ball.get_lives(); i++) {
            cv::circle(canvas, cv::Point2f(200 + (i * 55), 50), 25, cv::Scalar(0, 255, 0), cv::FILLED);
            }
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

void CBrickBreaker::reset() {
   _ball = CBall(_size);
   _paddle = CPaddle(_size);
   _bricks.clear();
   level_create(1);
   _score = 0;
   std::cout << "game reset" << std::endl;
   _game_over = false;
   _reset = false;
   }

void CBrickBreaker::level_create(int level_select) {
   /////////////////////////////////////ChatGPT
   const int maxY = 500;  // Maximum y value for brick centers
   if (level_select == 1) {
      // Define a 13x21 pattern where:
      // - '1' means a brick will be placed.
      // - '0' means no brick.
      // The eyes (rows 2 and 3) are filled, while the smile is drawn only as an outline.
      std::vector<std::string> pattern = {
          "000000000000000000000",  // row 0: blank
          "000000000000000000000",  // row 1: blank
          "000110000000000011000",  // row 2: eyes
          "000110000000000011000",  // row 3: eyes
          "000000000000000000000",  // row 4: blank
          "000000000000000000000",  // row 5: blank
          "000000111111111000000",  // row 6: top of smile outline (filled)
          "000011000000001100000",  // row 7: smile sides (only side bricks)
          "000010000000000100000",  // row 8: smile sides (only side bricks)
          "000011000000001100000",  // row 9: smile sides (only side bricks)
          "000000111111111000000",  // row 10: bottom of smile outline (filled)
          "000000000000000000000",  // row 11: blank
          "000000000000000000000"   // row 12: blank
         };

      int numRows = pattern.size();      // Should be 13
      int numCols = pattern[0].size();     // Should be 21

      // Loop over each cell in the pattern grid.
      for (int row = 0; row < numRows; row++) {
         for (int col = 0; col < numCols; col++) {
            if (pattern[row][col] == '1') {
               // Compute the brick's center position.
               // Bricks are assumed to be BRICK_WIDTH x BRICK_HEIGHT.
               // The y coordinate is offset by 100 pixels.
               cv::Point2f pos(
                  (col * BRICK_WIDTH) + (BRICK_WIDTH / 2.0f),
                  100 + (row * BRICK_HEIGHT) + (BRICK_HEIGHT / 2.0f)
               );
               _bricks.push_back(CBricks(_size, pos));
               }
            }
         }
      }
   else if (level_select == 2) {
      // Level 2: Checkerboard Pattern
      int numCols = _size.width / BRICK_WIDTH;
      int numRows = (_size.height - 100) / BRICK_HEIGHT;
      for (int row = 0; row < numRows; row++) {
         for (int col = 0; col < numCols; col++) {
            cv::Point2f pos(
               (col * BRICK_WIDTH) + (BRICK_WIDTH / 2.0f),
               100 + (row * BRICK_HEIGHT) + (BRICK_HEIGHT / 2.0f)
            );
            if ((row + col) % 2 == 0 && pos.y <= maxY)
               _bricks.push_back(CBricks(_size, pos));
            }
         }
      }
   else if (level_select == 3) {
      // Level 3: Diagonal Stripe Pattern
      int numCols = _size.width / BRICK_WIDTH;
      int numRows = (_size.height - 100) / BRICK_HEIGHT;
      for (int row = 0; row < numRows; row++) {
         int col = row % numCols; // Wrap around if necessary.
         cv::Point2f pos(
            (col * BRICK_WIDTH) + (BRICK_WIDTH / 2.0f),
            100 + (row * BRICK_HEIGHT) + (BRICK_HEIGHT / 2.0f)
         );
         if (pos.y <= maxY)
            _bricks.push_back(CBricks(_size, pos));
         }
      }
   else if (level_select == 4) {
      // Level 4: Plus Sign Pattern
      int centerX = _size.width / 2;
      int centerY = 100 + ((_size.height - 100) / 2);

      // Horizontal bar: bricks across the width.
      int numCols = _size.width / BRICK_WIDTH;
      for (int col = 0; col < numCols; col++) {
         cv::Point2f pos(
            (col * BRICK_WIDTH) + (BRICK_WIDTH / 2.0f),
            centerY
         );
         if (pos.y <= maxY)
            _bricks.push_back(CBricks(_size, pos));
         }
      // Vertical bar: bricks from top (100) to maxY.
      int numRows = (maxY - 100) / BRICK_HEIGHT;
      for (int row = 0; row < numRows; row++) {
         cv::Point2f pos(
            centerX,
            100 + (row * BRICK_HEIGHT) + (BRICK_HEIGHT / 2.0f)
         );
         if (pos.y <= maxY)
            _bricks.push_back(CBricks(_size, pos));
         }
      }
   ///////////////////////////////////////
   if (level_select == 5) {
      for (int i = 0; i <= 20; i++) {
         for (int j = 0; j <= 12; j++)
            _bricks.push_back(CBricks(_size, cv::Point2f(
            25+ (BRICK_WIDTH*i),100 + (BRICK_HEIGHT * j))));
         }
      }

   }