#pragma once

#include "stdafx.h"
#include "CBBObjects.h"

class CBall : public CBBObjects {
private:
   int _radius;
public:
   CBall(cv::Size size);
   ~CBall();
   bool collide_brick(CBBObjects& obj);
   bool collide_paddle(CBBObjects& obj);
   bool collide_wall(cv::Size size, cv::Point2f paddle_position);
   void draw(cv::Mat& im);
   int get_rad() const { return _radius; }
   void set_rad(int radius) { _radius = radius; }
   };