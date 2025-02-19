#pragma once
#include "stdafx.h"

class CGameObject {
protected:
   cv::Point2f _position;
   cv::Point2f _velocity;
   int _radius;
   int _lives;
public:
   void move();
   bool collide(CGameObject& obj);
   bool collide_wall(cv::Size board);
   void hit();
   int get_lives() const { return _lives; }
   void set_lives(int lives) { _lives = lives; }
   int get_radius() { return _radius; }
   void set_radius(int radius) { _radius = radius; }
   void set_pos(cv::Point2f pos) { _position = pos; }
   cv::Point2f get_pos() { return _position; }
   cv::Point2f get_vel() { return _velocity; }
   void draw(cv::Mat& im);
   void draw(cv::Mat& im, cv::Scalar color);
   float distance_squared(cv::Point2f a, cv::Point2f b);
   };
