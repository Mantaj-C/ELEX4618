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
   float delta_x_squared = (_position.x - obj._position.x) * (_position.x - obj._position.x);
   float delta_y_squared = (_position.y - obj._position.y) * (_position.y - obj._position.y);
   float distance_squared = delta_x_squared + delta_y_squared;
   float sum_radius_squared = (_radius + obj._radius) * (_radius + obj._radius);

   if (sum_radius_squared >= distance_squared)
      return true;
   else
      return false;
   }

bool CGameObject::collide_wall(cv::Size board) {
   int closestX = std::clamp(_position.x, 0, board.width); //ChatGPT partially
   int closestY = std::clamp(_position.y, 0, board.height); //ChatGPT partially
   int delta_x_squared = (_position.x - closestX) * (_position.x - closestX);
   int delta_y_squared = (_position.y - closestY) * (_position.y - closestY);
   float distance = std::sqrt(delta_x_squared + delta_y_squared);

   if (_radius >= distance)
      return true;
   else
      return false;
   }

void CGameObject::draw(cv::Mat& im, int linetype) {
   cv::circle(im, _position, _radius, cv::Scalar(0,0,255), linetype);
   }