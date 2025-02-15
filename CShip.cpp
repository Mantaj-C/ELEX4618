#include "CShip.h"
#include <cmath>

#define TILT_SPEED 5

enum { TIP = 0, LEFT_CORNER, NOTCH, RIGHT_CORNER };
enum { TILT_LEFT = -1, TILT_RIGHT = 1 };

CShip::CShip(cv::Size size) {
   _lives = 10;
   _position = cv::Point2f(size.width / 2, size.height / 2);
   _velocity = cv::Point2f(0, 0);
   _shape.push_back(cv::Point(0, -10));
   _shape.push_back(cv::Point(-10 , 10));
   _shape.push_back(cv::Point(0, 5));
   _shape.push_back(cv::Point(10, 10));
   }

CShip::~CShip() {

   }

bool CShip::collide(CGameObject& obj) { //ChatGPT partially
   int sides = _shape.size();
   for (int i = 0; i < sides; ++i) {
      cv::Point v1 = shape[i];
      cv::Point v2 = shape[(i + 1) % sides];
      //v1 = shape[i]  v2 = shape[(i + 1) % sides]
      //circlecenter = p,v1 = v,v2 = w
      float squared_distance = distance_squared(v1, v2);
      if (squared_distance == 0)
         if (distance_squared(obj._position, v1) <= (obj._radius * obj._radius))
            return true;
      float projection_factor = ((obj._position.x - v1.x) * (v2.x - v1.x) +
         (obj._position.y - v1.y) * (v2.y - v1.y)) / squared_distance;
      cv::Point2f projection(v1.x + projection_factor * (v2.x - v1.x),
         v1.y + projection_factor * (v2.y - v1.y));
      if (distance_squared(obj._position, projection) <= (obj._radius * obj._radius))
         return true;
      }
   return false;
   }

void CShip::draw(cv::Mat& im) {
   std::vector<cv::Point> _shape_postitioned = { _shape[TIP] + _position,
      _shape[LEFT_CORNER] + _position, _shape[NOTCH] + _position, _shape[RIGHT_CORNER] + _position }
   std::vector<std::vector<cv::Point>> polygon = { _shape_postitioned }; //ChatGPT
   cv::polylines(im, polygon, true, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
   }

void CShip::tilt(int direction) {
   float angle = direction * TILT_SPEED * CV_PI / 180;
   float cosTheta = std::cos(angle);
   float sinTheta = std::sin(angle);

   for (int i = 0; i <= _shape.size(); ++i) {
      cv::Point2f rotated = _shape[i];
      rotated.x = _shape[i].x * cosTheta - _shape[i].y * sinTheta; //ChatGPT
      rotated.y = _shape[i].x * sinTheta + _shape[i].y * cosTheta; //ChatGPT
      _shape[i] = rotated;
      }
   }