#pragma once

#include "stdafx.h"
#include "CGameObject.h"
class CShip : public CGameObject {
private:
   std::vector<cv::Point2f> _shape;
public:
   CShip(cv::Size size);
   ~CShip();
   bool collide(CGameObject& obj);
   int get_shape(int point) { return _shape[point]; }
   void set_shape(int point, cv::Point position) { _shape[point] = position; }
   void draw(cv::Mat& im);
   void tilt(int direction);
   };