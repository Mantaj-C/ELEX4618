#include "stdafx.h"
#include "CAsteroid.h"
#include <random>

#define MAX_RADIUS 70
#define MIN_RADIUS 15
#define VELOCITY 3.5
#define VELOCITY_OFFSET 25
#define SPIN_SPEED 1
#define SCALE_FACTOR 1.2

enum { TOP_SCREEN = 0, LEFT_SCREEN, BOTTOM_SCREEN, RIGHT_SCREEN };

CAsteroid::CAsteroid(cv::Size size) {
   std::random_device rd; //ChatGPT
   std::mt19937 engine(rd()); //ChaptGPT
   std::uniform_int_distribution<int> distribution1(MIN_RADIUS, MAX_RADIUS); //ChatGPT
   _radius = distribution1(engine);
   //if (_radius >= (MAX_RADIUS - MIN_RADIUS))
      //_lives = 2;
   //else
      _lives = 1;
      _angle = 0.0;

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

CAsteroid::~CAsteroid() {
     }

CAsteroid::CAsteroid(cv::Point2f position, cv::Point2f velocity, int radius) {
   _radius = radius;
   _position = position;
   _velocity = velocity;
   _lives = 1;
   _angle = 0.0;
   }
///////////////////////////////////////////////////////////////////////////ChatGPT
void CAsteroid::draw(cv::Mat& im, cv::Mat& aster) {
   // Increment the angle.
   _angle += SPIN_SPEED;
   if (_angle >= 360.0f)
      _angle -= 360.0f;

   // Ensure _radius is positive.
   if (_radius <= 0)
      return;

   // Calculate the drawn size using the scaling factor.
   int drawSize = static_cast<int>(2 * _radius * SCALE_FACTOR);

   // Cache the resized asteroid texture if not already cached or if the size has changed.
   if (_cachedResized.empty() || _cachedResized.cols != drawSize || _cachedResized.rows != drawSize) {
      cv::resize(aster, _cachedResized, cv::Size(drawSize, drawSize));
      }
   if (_cachedResized.empty())
      return;

   // Determine the center of the cached image.
   cv::Point2f center(static_cast<float>(_cachedResized.cols) / 2.0f,
      static_cast<float>(_cachedResized.rows) / 2.0f);

   // Get the rotation matrix around the center.
   cv::Mat rotMat = cv::getRotationMatrix2D(center, _angle, 1.0);

   // Rotate the cached image.
   cv::Mat rotated;
   cv::warpAffine(_cachedResized, rotated, rotMat, _cachedResized.size(),
      cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0, 0));

   // Compute the destination rectangle so that the rotated image is centered at _position.
   int left = static_cast<int>(_position.x - rotated.cols / 2);
   int top = static_cast<int>(_position.y - rotated.rows / 2);
   cv::Rect desiredRect(left, top, rotated.cols, rotated.rows);

   // Compute the intersection between the desired rect and the canvas.
   cv::Rect imageRect(0, 0, im.cols, im.rows);
   cv::Rect drawRect = desiredRect & imageRect;
   if (drawRect.empty())
      return;

   // Compute the corresponding ROI in the rotated image.
   int offsetX = drawRect.x - desiredRect.x;
   int offsetY = drawRect.y - desiredRect.y;
   cv::Rect textureROI(offsetX, offsetY, drawRect.width, drawRect.height);
   textureROI = textureROI & cv::Rect(0, 0, rotated.cols, rotated.rows);
   if (textureROI.empty())
      return;

   cv::Mat subSrc = rotated(textureROI);
   cv::Mat subDst = im(drawRect);

   // Perform alpha blending if the rotated image has an alpha channel.
   if (rotated.channels() == 4) {
      std::vector<cv::Mat> channels;
      cv::split(subSrc, channels);
      cv::Mat alpha = channels[3];
      channels.pop_back();
      cv::Mat color;
      cv::merge(channels, color);
      color.copyTo(subDst, alpha);
      }
   else {
      // Otherwise, just copy the pixels.
      subSrc.copyTo(subDst);
      }
}
//////////////////////////////////////////////////////////////////////////////