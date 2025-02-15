#pragma once

#include "stdafx.h"
#include "CGameObject.h"
#include "Rainbowcolors.h"
#include <chrono>

class CShip : public CGameObject {
private:
   std::vector<cv::Point2f> _shape;
   double _last_thrust;
   bool _thruster_active;
   std::array<cv::Scalar, 100> _rainbow;
   int _rainbow_index;
public:
   CShip(cv::Size size);
   ~CShip();
   bool collide(CGameObject& obj);
   cv::Point2f get_shape(int point) { return _shape[point]; }
   void set_shape(int point, cv::Point position) { _shape[point] = position; }
   void draw(cv::Mat& im);
   void draw(cv::Mat& im,cv::Point2f position);
   void tilt(int direction);
   void move(bool thurst);
   };