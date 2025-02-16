#include "stdafx.h"
#include "CInvader.h"
#include <random>

#define VELOCITY 3
#define VELOCITY_OFFSET 25

enum { TOP_SCREEN = 0, LEFT_SCREEN, BOTTOM_SCREEN, RIGHT_SCREEN };

CInvader::CInvader(cv::Size size) {
   std::random_device rd; //ChatGPT
   std::mt19937 engine(rd()); //ChaptGPT
   _radius = 20;
   _lives = 1;
   std::uniform_int_distribution<int> distribution2(TOP_SCREEN, RIGHT_SCREEN);
   int area_select = distribution2(engine);
   if (area_select == TOP_SCREEN || area_select == BOTTOM_SCREEN) {
      std::uniform_int_distribution<int> distribution3(0, size.width);
      if (area_select == TOP_SCREEN)
         _position = cv::Point2f(distribution3(engine), -_radius + 2);
      else
         _position = cv::Point2f(distribution3(engine), size.height + _radius - 2);
      }
   else {
      std::uniform_int_distribution<int> distribution4(0, size.height);
      if (area_select == LEFT_SCREEN)
         _position = cv::Point2f(-_radius + 2, distribution4(engine));
      else
         _position = cv::Point2f(size.width + _radius - 2, distribution4(engine));
      }
   cv::Point2f center(500, 400);
   cv::Point2f direction = center - _position;
   float velocity_magnitude = std::sqrt(distance_squared(center, _position));
   cv::Point2f velocity_direct = VELOCITY * direction / velocity_magnitude;
   std::uniform_int_distribution<int> distribution5(-VELOCITY_OFFSET, VELOCITY_OFFSET);
   float angle = distribution5(engine) * CV_PI / 180;
   _velocity.x = velocity_direct.x * std::cos(angle) - velocity_direct.y * std::sin(angle);
   _velocity.y = velocity_direct.x * std::sin(angle) + velocity_direct.y * std::cos(angle);
   }

CInvader::~CInvader() {

   }

void CInvader::draw(cv::Mat& im) {
   cv::line(im, _position, _position + cv::Point2f(10, -30), cv::Scalar(0, 255, 0), 2);
   cv::line(im, _position, _position + cv::Point2f(-10, -30), cv::Scalar(0, 255, 0), 2);
   cv::circle(im, _position, _radius, cv::Scalar(0, 255, 0), cv::FILLED);
   cv::circle(im, _position + cv::Point2f(10, -10), 3, cv::Scalar(0, 0, 0), cv::FILLED);
   cv::circle(im, _position + cv::Point2f(-10, -10), 3, cv::Scalar(0, 0, 0), cv::FILLED);
   }