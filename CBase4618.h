#pragma once

#include "stdafx.h"
#include "CControl.h"


class CBase4618 {
private:
   CControl _control;
   cv::Mat _canvas;
   bool _exit;
public:
   CBase4618();
   ~CBase4618();
   virtual void gpio() = 0;
   virtual void update() = 0;
   virtual void draw() = 0;
   void run();
   CControl& getControl();
   void setControl(const CControl& control);
   cv::Mat& getCanvas();
   void setCanvas(const cv::Mat& canvas);
   bool getExit();
   void setExit(const bool& exit);
   };