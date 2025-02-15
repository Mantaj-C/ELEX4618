#include "CMissile.h"

#define VELOCITY 3

CMissile::CMissile(cv::Point2f starting_point, cv::Point2f direction_point) {
   _lives = 1;
   _radius = 1;
   cv::Point2f velocity_vector = starting_point - direction_point;
   float velocity_magnitude = std::sqrt(distance_squared(direction_point, starting_point));
   _velocity = VELOCITY * velocity_vector / velocity_magnitude;
   _position = starting_point
   }

CMissile::~CMissile() {

   }