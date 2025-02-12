/**
 * @file CSnakeGameV2.h
 * @brief Declaration of the CSnakeGameV2 class, providing game logic for a Snake game implementation.
 */

#pragma once

#include "stdafx.h"
#include "CBase4618.h"
#include "RainbowColors.h"
#include <vector>
#include <deque>
#include <mutex>

 /**
  * @class CSnakeGameV2
  * @brief A class representing the Snake game, extending the CBase4618 class.
  *
  * This class handles the game logic, including rendering, user input (via GPIO and joystick),
  * LED color synchronization, apple movement, and laser mechanics.
  */
class CSnakeGameV2 : public CBase4618 {
private:
   /**
    * @brief Structure representing color properties.
    */
   struct _color_structure {
      std::string _color; ///< Color name (e.g., "Red", "Blue").
      cv::Scalar _color_scalar; ///< OpenCV color representation.
      int _color_pin; ///< GPIO pin associated with the color.
      };

   cv::Size _size; ///< Game board size.
   bool _reset; ///< Flag to reset the game state.
   int _color_index; ///< Index to track the current color.
   std::vector<_color_structure> _colorArray; ///< Array of available colors.
   std::deque<cv::Point> _snake_position; ///< Deque storing the snake's position.
   cv::Point2f _joystick_position; ///< Stores joystick position (x, y).
   int _prev_color; ///< Previous selected color index.
   bool _button_pressed; ///< Flag for button press status.
   bool _color_flag; ///< Flag for color state.
   cv::Point _direction; ///< Stores the snake's movement direction.
   float _FPS; ///< Frames per second calculation.
   int _frameCount; ///< Counter for frames rendered.
   double _lastTime_frame; ///< Timestamp of last frame update.
   double _lastTime_apple; ///< Timestamp of last apple spawn.
   double _lastTime_lazer; ///< Timestamp of last laser firing.
   bool _musicplaying; ///< Flag for music playback status.
   int _step_size; ///< The step size of the snake movement.
   int _snake_speed; ///< Speed of the snake.
   int _snake_length; ///< Current length of the snake.
   bool _apple_eaten; ///< Flag indicating if an apple has been eaten.
   std::deque<cv::Rect> _apple_position; ///< Positions of apples on the board.
   std::deque<cv::Point> _apple_direction; ///< Movement direction of apples.
   int _score; ///< Player's current score.
   cv::Rect _snake_hit_box; ///< Bounding box of the snake's head.
   bool _game_over; ///< Flag indicating if the game is over.
   std::mutex _music_mutex; ///< Mutex for thread-safe music control.
   int _update_time; ///< Time taken for update function execution.
   int _gpio_time; ///< Time taken for GPIO function execution.
   int _draw_time; ///< Time taken for draw function execution.
   cv::Mat _overlay; ///< Overlay image for rendering.
   float _apple_spawn_rate; ///< Rate at which apples spawn.
   std::deque<cv::Point> _lazer_position; ///< Positions of active lasers.
   std::deque<cv::Point> _lazer_direction; ///< Directions of active lasers.
   cv::Mat _bottom_ship; ///< Image for bottom ship rendering.
   cv::Mat _right_ship; ///< Image for right ship rendering.
   bool _reset_done; ///< Flag indicating if reset is completed.
   bool _lazer_firing; ///< Flag for laser firing state.
   bool _lazer_drawn; ///< Flag to track if laser has been drawn.
   bool _apple_movement; ///< Flag to indicate apple movement state.
   bool _lazer_on; ///< Flag to indicate if the laser is active.
   std::array<cv::Scalar, 100> _rainbow; ///< Array storing rainbow color gradients.

public:

   /**
    * @brief Constructor for CSnakeGameV2.
    * @param size The size of the game board.
    */
   CSnakeGameV2(cv::Size size);

   /**
    * @brief Destructor for CSnakeGameV2.
    */
   ~CSnakeGameV2();

   /**
    * @brief Handles GPIO operations, such as button inputs and joystick control.
    */
   void gpio() override;

   /**
    * @brief Updates the game logic, including snake movement and collision detection.
    */
   void update() override;

   /**
    * @brief Draws the game elements (snake, apples, lasers, etc.) onto the screen.
    */
   void draw() override;

   /**
    * @brief Resets the game state, including the snake position, score, and objects.
    */
   void reset();

   /**
    * @brief Spawns apples at random locations.
    */
   void apple_spawn();

   /**
    * @brief Manages the logic of the snake's movement and apple interactions.
    * @param apple_flag Determines whether the apple has been eaten.
    */
   void snake_vector_logic(bool apple_flag);

   /**
    * @brief Handles laser mechanics, including firing and movement.
    */
   void lazer();
   };
