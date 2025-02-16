#pragma once

#include "stdafx.h"
#include "CGameObject.h"

class CInvader : public CGameObject {
public:
   CInvader(cv::Size size);
   ~CInvader();
   void draw(cv::Mat& im);
   };