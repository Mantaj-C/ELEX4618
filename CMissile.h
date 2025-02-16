#pragma once

#include "stdafx.h"
#include "CGameObject.h"
class CMissile : public CGameObject {
public:
   CMissile(cv::Point2f starting_point, cv::Point2f direction_point);
   CMissile(cv::Point2f starting_point, cv::Point2f direction_point, float velocity);
   ~CMissile();
   };