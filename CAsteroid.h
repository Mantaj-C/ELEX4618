#pragma once

#include "stdafx.h"
#include "CGameObject.h"
class CAsteroid : public CGameObject {
private:
   float _angle;
   cv::Mat _cachedResized;
public:
   CAsteroid(cv::Size size);
   CAsteroid(cv::Point2f position,cv::Point2f velocity,int radius);
   ~CAsteroid();
   void draw(cv::Mat& im, cv::Mat& aster);
   };