#include "CAsteroid.h"
#include <random>

#define MAX_RADIUS 50
#define MIN_RADIUS 5
#define VELOCITY 5
#define VELOCITY_OFFSET 15

enum { TOP_SCREEN = 0, LEFT_SCREEN, BOTTOM_SCREEN, RIGHT_SCREEN };

CAsteroid::CAsteroid(cv::Size size) {
   std::random_device rd; //ChatGPT
   std::mt19937 engine(rd()); //ChaptGPT
   std::uniform_int_distribution<int> distribution1(MIN_RADIUS, MAX_RADIUS); //ChatGPT
   _radius = distribution1(engine);
   if (_radius >= (MAX_RADIUS * (2 / 3)))
      _lives = 3;
   else if (_radius >= (MAX_RADIUS / 3))
      _lives = 2;
   else
      _lives = 1;

   std::uniform_int_distribution<int> distribution2(TOP_SCREEN, RIGHT_SCREEN);
   int area_select = distribution2(engine);
   if (area_select == TOP_SCREEN || area_select == BOTTOM_SCREEN) {
      std::uniform_int_distribution<int> distribution3(0, size.width);
      if (area_select == TOP_SCREEN)
         _position = cv::Point2f(distribution3(engine), -_radius + 2);
      else
         _position = cv::Point2f(distribution3(engine), _size.height + _radius - 2);
      }
   else {
      std::uniform_int_distribution<int> distribution4(0, size.height);
      if (area_select == LEFT_SCREEN)
         _position = cv::Point2f(-_radius + 2, distribution4(engine));
      else 
         _position = cv::Point2f(size.width + _radius - 2, distribution4(engine));
      }

   float velocity_magnitude = std::sqrt(distance_squared(0, _position));
   cv::Point2f velocity_direct = VELOCITY * -_position / velocity_magnitude;
   std::uniform_int_distribution<int> distribution5(-VELOCITY_OFFSET, VELOCITY_OFFSET);
   float angle = distribution2(engine) * CV_PI / 180;
   _velocity.x = velocity_direct.x * std::cos(angle) - velocity_direct.y * std::sin(angle);
   _velocity.y = velocity_direct.x * std::sin(angle) + velocity_direct.y * std::cos(angle);
   }

CAsteroid::~CAsteroid() {

   }