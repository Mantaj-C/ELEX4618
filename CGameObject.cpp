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
   if ((_position.x + _radius) < 0 ||
      (_position.x - _radius) > board.width ||
      (_position.y + _radius) < 0 ||
      (_position.y - _radius) > board.height)
      return true;
   else
      return false;
   }

void CGameObject::draw(cv::Mat& im) {
   cv::circle(im, _position, _radius, cv::Scalar(0,0,255),2);
   }

void CGameObject::draw(cv::Mat& im,cv::Scalar color) {
   cv::circle(im, _position, _radius, color, 2);
   }

float CGameObject::distance_squared(cv::Point2f a, cv::Point2f b) {
   float delta_x = a.x - b.x;
   float delta_y = a.y - b.y;
   return (delta_x * delta_x) + (delta_y * delta_y);
   }