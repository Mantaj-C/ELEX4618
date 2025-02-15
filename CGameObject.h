#pragma once
#include "stdafx.h"

class CGameObject {
protected:
   cv::Point2f _position;
   cv::Point2f _velocity;
   int _radius;
   int _lives;
   int _tilt;
public:
   void move();
   bool collide(CGameObject& obj);
   bool collide_wall(cv::Size board);
   void hit();
   int get_lives() { return _lives; }
   void set_lives(int lives) { _lives = lives; }
   void set_pos(Point2f pos) { _position = pos; }
   cv::Point2f get_pos() { return _position; }
   int get_tilt() { return _tilt; }
   void set_tilt(int tilt) { _tilt = tilt; }
   void draw(cv::Mat& im);
   float distance_squared(cv::Point a, cv::Point b);
   };
