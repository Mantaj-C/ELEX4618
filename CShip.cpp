#include "stdafx.h"
#include "CShip.h"
#include <cmath>

#define TILT_SPEED 5
#define VELOCITY 5
#define DEACCELERATION_SPEED 0.5
#define SHIP_SCALING 1.0
#define FLAME_OFFSET 10.0f
#define FLAME_SIZE 15.0f
#define SHIP_LIVES_IMAGE_SCALING 2.5

enum { TIP = 0, LEFT_CORNER, NOTCH, RIGHT_CORNER };
enum { TILT_LEFT = -1, TILT_RIGHT = 1 };

CShip::CShip(cv::Size size) {
   _lives = 10;
   _position = cv::Point2f(size.width / 2, size.height / 2);
   _velocity = cv::Point2f(0, 0);
   _shape.push_back(cv::Point2f(0, -10 * SHIP_SCALING));
   _shape.push_back(cv::Point2f(-10 * SHIP_SCALING, 10 * SHIP_SCALING));
   _shape.push_back(cv::Point2f(0, 5 * SHIP_SCALING));
   _shape.push_back(cv::Point2f(10 * SHIP_SCALING, 10 * SHIP_SCALING));
   _thruster_active = false;
   _rainbow = RainbowColors::getRainbowColors();
   _rainbow_index = 0;
   }

CShip::~CShip() {

   }

////////////////////////////////////////ChatGPT
bool CShip::collide(CGameObject& obj) {
   // Build the ship’s polygon in world coordinates.
   std::vector<cv::Point2f> poly;
   for (size_t i = 0; i < _shape.size(); i++) {
      poly.push_back(_shape[i] + _position);
      }

   // First, check if the circle's center is inside the polygon.
   // cv::pointPolygonTest returns a positive value if the point is inside.
   if (cv::pointPolygonTest(poly, obj.get_pos(), false) >= 0)
      return true;

   // Otherwise, check the distance from the circle's center to each edge.
   int sides = poly.size();
   for (int i = 0; i < sides; ++i) {
      cv::Point2f v1 = poly[i];
      cv::Point2f v2 = poly[(i + 1) % sides];

      // Compute the squared length of the edge.
      float edgeLengthSq = (v2.x - v1.x) * (v2.x - v1.x) +
         (v2.y - v1.y) * (v2.y - v1.y);
      if (edgeLengthSq == 0) {
         // Degenerate edge: check distance from the circle's center to v1.
         float dx = obj.get_pos().x - v1.x;
         float dy = obj.get_pos().y - v1.y;
         if ((dx * dx + dy * dy) <= (obj.get_radius() * obj.get_radius()))
            return true;
         continue;
         }

      // Projection factor: where the projection of obj.get_pos() lies on the edge.
      float projection = ((obj.get_pos().x - v1.x) * (v2.x - v1.x) +
         (obj.get_pos().y - v1.y) * (v2.y - v1.y)) / edgeLengthSq;
      // Clamp projection between 0 and 1 so that the projection lies on the segment.
      projection = std::max(0.0f, std::min(1.0f, projection));

      // Compute the closest point on the edge.
      cv::Point2f closestPoint = v1 + projection * (v2 - v1);
      float dx = obj.get_pos().x - closestPoint.x;
      float dy = obj.get_pos().y - closestPoint.y;
      float distSq = dx * dx + dy * dy;

      if (distSq <= (obj.get_radius() * obj.get_radius()))
         return true;
      }

   return false;
   }
///////////////////////////////////////

void CShip::draw(cv::Mat& im) {
   std::vector<cv::Point> _shape_postitioned = { _shape[TIP] + _position,
      _shape[LEFT_CORNER] + _position, _shape[NOTCH] + _position, _shape[RIGHT_CORNER] + _position };
   std::vector<std::vector<cv::Point>> polygon = { _shape_postitioned }; //ChatGPT
   cv::polylines(im, polygon, true, cv::Scalar(255, 0, 0), 2, cv::LINE_AA);

   ////////////////////////////////////////ChatGPT
   if (_thruster_active) {
      // Compute world positions of tip and notch.
      cv::Point2f tipPos = _shape[TIP] + _position;
      cv::Point2f notchPos = _shape[NOTCH] + _position;

      // Compute forward direction (from notch to tip) and normalize.
      cv::Point2f forward = tipPos - notchPos;
      float norm = std::sqrt(forward.x * forward.x + forward.y * forward.y);
      if (norm != 0) {
         forward.x /= norm;
         forward.y /= norm;
         }

      // Rearward direction is the opposite of forward.
      cv::Point2f rear = -forward;

      // Determine flame origin: offset from the notch by FLAME_OFFSET in the rear direction.
      cv::Point2f flameOrigin = notchPos + rear * FLAME_OFFSET;

      // Compute the angle of the rear vector (in radians).
      float flame_angle = std::atan2(rear.y, rear.x) - CV_PI / 2;

      // Define the flame polygon in a local coordinate system (pointing downwards).
      // (0,0) is the flame base. In this default coordinate system, "down" (positive y) is the flame direction.
      std::vector<cv::Point2f> localFlamePoints;
      localFlamePoints.push_back(cv::Point2f(0, 0));                          // Base of flame.
      localFlamePoints.push_back(cv::Point2f(-FLAME_SIZE / 2.0f, FLAME_SIZE));     // Left tip.
      localFlamePoints.push_back(cv::Point2f(0, FLAME_SIZE + FLAME_SIZE / 2.0f));  // Bottom tip.
      localFlamePoints.push_back(cv::Point2f(FLAME_SIZE / 2.0f, FLAME_SIZE));      // Right tip.

      // Rotate each local flame point by flame_angle and translate to flameOrigin.
      std::vector<cv::Point> flamePoints;
      for (const auto& pt : localFlamePoints) {
         float rotatedX = pt.x * std::cos(flame_angle) - pt.y * std::sin(flame_angle);
         float rotatedY = pt.x * std::sin(flame_angle) + pt.y * std::cos(flame_angle);
         flamePoints.push_back(cv::Point(static_cast<int>(rotatedX + flameOrigin.x),
            static_cast<int>(rotatedY + flameOrigin.y)));
         }

      // Draw the flame polygon.
      cv::fillConvexPoly(im, flamePoints, _rainbow[_rainbow_index], cv::LINE_AA);

      // Optionally reset the thruster flag.
      _thruster_active = false;

      if (_rainbow_index == 99)
         _rainbow_index = 0;
      else
         _rainbow_index++;
      }
   //////////////////////////////////////////
   }

void CShip::tilt(int direction) {
   float angle = direction * TILT_SPEED * CV_PI / 180;
   float cosTheta = std::cos(angle);
   float sinTheta = std::sin(angle);

   for (int i = 0; i < _shape.size(); ++i) {
      cv::Point2f rotated = _shape[i];
      rotated.x = _shape[i].x * cosTheta - _shape[i].y * sinTheta; //ChatGPT
      rotated.y = _shape[i].x * sinTheta + _shape[i].y * cosTheta; //ChatGPT
      _shape[i] = rotated;
      }
   }

void CShip::move(bool thrust) {
   cv::Point2f velocity_vector = _shape[TIP] - _shape[NOTCH];
   float velocity_magnitude = std::sqrt(distance_squared(_shape[TIP], _shape[NOTCH]));
   if (thrust) {
      // When thrust is applied, reset velocity and update the last thrust time.
      _velocity = VELOCITY * velocity_vector / velocity_magnitude;
      _last_thrust = cv::getTickCount() / cv::getTickFrequency();
      _thruster_active = true;
      }
   else {
      // Calculate how long it's been since the last thrust.
      double currentTime = cv::getTickCount() / cv::getTickFrequency();
      double time_passed = currentTime - _last_thrust;

      // Exponential deceleration.
      float temp_velocity = VELOCITY * std::exp(-time_passed * DEACCELERATION_SPEED);

      // Clamp small velocities to zero (you can adjust the threshold as needed).
      if (temp_velocity < 0.1f) {
         temp_velocity = 0.0f;
         }

      // Update the velocity in the same direction.
      _velocity = temp_velocity * velocity_vector / velocity_magnitude;
      }

   cv::Point2f temp_position = (_position + _velocity);
   if (temp_position.x > 1000)
      _position += cv::Point2f(-1000, 0);
   else if (temp_position.x < 0)
      _position += cv::Point2f(1000, 0);
   else if (temp_position.y > 800)
      _position += cv::Point2f(0, -800);
   else if (temp_position.y < 0)
      _position += cv::Point2f(0, 800);
   else
      _position += _velocity;

   }

void CShip::draw(cv::Mat& im, cv::Point2f position) {
   std::vector<cv::Point> _shape_postitioned = { (_shape[TIP] * SHIP_LIVES_IMAGE_SCALING)
      + position, (_shape[LEFT_CORNER] * SHIP_LIVES_IMAGE_SCALING)
      + position, (_shape[NOTCH] * SHIP_LIVES_IMAGE_SCALING)
      + position, (_shape[RIGHT_CORNER] * SHIP_LIVES_IMAGE_SCALING) + position };
   std::vector<std::vector<cv::Point>> polygon = { _shape_postitioned }; //ChatGPT
   cv::polylines(im, polygon, true, cv::Scalar(255, 0, 0), 2, cv::LINE_AA);
   }
