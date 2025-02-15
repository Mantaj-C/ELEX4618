#include "stdafx.h"
#include "CGameObject.h"
#include <algorithm>

void CGameObject::move() {
   _position += _velocity;
   }

void CGameObject::hit() {
   _lives -= 1;
   }

bool CGameObject::collide(CGameObject& obj) {
   float squared_distance = distance_squared(_position, obj._position);
   float sum_radius_squared = (_radius + obj._radius) * (_radius + obj._radius);

   if (sum_radius_squared >= squared_distance)
      return true;
   else
      return false;
   }

bool CGameObject::collide_wall(cv::Size board) {
   cv::Point2f closest_point = cv::Point2f(std::clamp(_position.x, 0, board.width),
      std::clamp(_position.y, 0, board.height)); //ChatGPT partially
   float squared_distance = distance_squared(_position,closest_point);

   if ((_radius * _radius) >= squared_distance)
      return true;
   else
      return false;
   }

void CGameObject::draw(cv::Mat& im) {
   cv::circle(im, _position, _radius, cv::Scalar(0,0,255));
   }

float CGameObject::distance_squared(cv::Point a, cv::Point b) {
   float delta_x = a.x - b.x;
   float delta_y = a.y - b.y;
   return (delta_x * delta_x) + (delta_y * delta_y);
   }