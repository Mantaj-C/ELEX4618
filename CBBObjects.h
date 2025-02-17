#pragma once

#include "stdafx.h"

#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 20

class CBBObjects {
protected:
   cv::Point2f _position;
   int _lives;
   cv::Rect2f _rectangle_size;
   cv::Point2f _velocity;
   
public:
   void hit();
   int get_lives() const { return _lives; }
   void set_lives(int lives) { _lives = lives; }
   void set_pos(cv::Point2f pos) { _position = pos; }
   cv::Point2f get_pos() { return _position; }
   void set_vel(cv::Point2f velocity) { _velocity = velocity; }
   cv::Point2f get_vel() { return _velocity; }
   void set_size(cv::Rect2f size) { _rectangle_size = size; }
   cv::Rect2f get_size() { return _rectangle_size; }
   void draw(cv::Mat& im,cv::Scalar color);
   void move();
   };