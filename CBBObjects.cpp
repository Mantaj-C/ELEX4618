#include "stdafx.h"
#include "CBBObjects.h"
#include <algorithm>

void CBBObjects::hit() {
   _lives -= 1;
   }

void CBBObjects::draw(cv::Mat& im,cv::Scalar color) {
   cv::rectangle(im, _rectangle_size, color, cv::FILLED);
   cv::rectangle(im, _rectangle_size, cv::Scalar(255,255,255));
   }

void CBBObjects::move() {
   _position += _velocity;
   }