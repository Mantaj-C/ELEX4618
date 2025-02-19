/**
 * @file CSnakeGame.h
 * @brief Declaration of the CSnakeGame class, providing game logic for a Snake game implementation.
 */

#pragma once

#include "stdafx.h"
#include "CBase4618.h"
#include <vector>

 /**
  * @class CSnakeGame
  * @brief A class representing the Snake game, extending the CBase4618 class.
  *
  * This class handles the game logic, including rendering, user input (via GPIO and joystick),
  * and LED color synchronization.
  */
class CSnakeGame : public CBase4618 {
private:
   /**
    * @struct _color_structure
    * @brief Structure to store color information for the game.
    *
    * This structure holds a color name, its corresponding `cv::Scalar` BGR value,
    * and the associated LED pin number for LED control.
    */
   struct _color_structure {
      /**
       * @brief Name of the color (e.g., "Red", "Green", "Blue").
       */
      std::string _color;

      /**
       * @brief BGR color representation for OpenCV.
       */
      cv::Scalar _color_scalar;

      /**
       * @brief GPIO pin number associated with the LED for this color.
       */
      int _color_pin;
      };

   /**
    * @brief The size of the game canvas.
    */
   cv::Size _size;

   /**
    * @brief Flag indicating whether the game should reset.
    */
   bool _reset;

   /**
    * @brief Index tracking the current selected color.
    */
   int _color_index;

   /**
    * @brief A list of available colors and their associated LED pins.
    */
   std::vector<_color_structure> _colorArray;

   /**
    * @brief The deque storing the positions of the snake's body.
    */
   std::deque<cv::Point> _snake_position;

   /**
    * @brief The joystick's last known position for controlling movement.
    */
   cv::Point2f _joystick_position;

   /**
    * @brief Stores the previous color before a color change occurs.
    */
   int _prev_color;

   /**
    * @brief Tracks whether the button is currently pressed.
    */
   bool _button_pressed;

   /**
    * @brief Flag indicating if the color should be updated.
    */
   bool _color_flag;

   /**
    * @brief The previous movement direction of the snake.
    */
   cv::Point _prev_direction;


   /**
    * @brief Frame rate of the game in frames per second.
    */
   float _FPS;

   /**
    * @brief Counter for the number of frames rendered.
    */
   int _frameCount;

   /**
    * @brief Timestamp of the last FPS update.
    */
   double _lastTime;

   /**
    * @brief Flag indicating whether music is currently playing.
    */
   bool _musicplaying;

public:
   /**
    * @brief Constructs the Snake game with a specified canvas size.
    * @param size The size of the game canvas.
    */
   CSnakeGame(cv::Size size);

   /**
    * @brief Destructor for CSnakeGame.
    */
   ~CSnakeGame();

   /**
    * @brief Handles GPIO operations such as button presses and LED updates.
    *
    * This function is responsible for processing user input and syncing LED colors.
    */
   void gpio() override;

   /**
    * @brief Updates the game state, including snake movement and color changes.
    */
   void update() override;

   /**
    * @brief Renders the game graphics, drawing the snake and user interface.
    */
   void draw() override;
   };
