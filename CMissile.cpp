#include "stdafx.h"
#include "CMissile.h"

#define VELOCITY 4.3

CMissile::CMissile(cv::Point2f starting_point, cv::Point2f direction_point) {
   _lives = 1;
   _radius = 4;
   cv::Point2f velocity_vector = starting_point - direction_point;
   float velocity_magnitude = std::sqrt(distance_squared(direction_point, starting_point));
   _velocity = VELOCITY * velocity_vector / velocity_magnitude;
   _position = starting_point;
   }

CMissile::CMissile(cv::Point2f starting_point, cv::Point2f direction_point, float velocity) {
   _lives = 1;
   _radius = 4;
   cv::Point2f velocity_vector = starting_point - direction_point;
   float velocity_magnitude = std::sqrt(distance_squared(direction_point, starting_point));
   _velocity = velocity * velocity_vector / velocity_magnitude;
   _position = direction_point;
   }

CMissile::~CMissile() {

   }