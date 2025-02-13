#include "CShip.h"

CShip::CShip(cv::Size size) {
   _lives = 10;
   _radius = 20;
   _position = cv::Point2f(size.width / 2, size.height / 2);
   _velocity = cv::Point2f(0, 0);
   }

CShip::~Cship() {

   }