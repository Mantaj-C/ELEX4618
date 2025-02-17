#include "stdafx.h"
#include "CBall.h"
#include <box2d/box2d.h>

CBall::CBall(cv::Size size) {
   _lives = 3;
   _velocity = cv::Point2f(0, 0);
   _radius = 8;
   _position = cv::Point2f(size.width / 2, size.height - PADDLE_HEIGHT - _radius - 2);
   }

CBall::~CBall() {

   }
///////////////////////////////////////////////////////////////////////ChatGPT
bool CBall::collide_brick(CBBObjects& obj) {
   // Create a Box2D circle shape for the ball.
   b2CircleShape circle;
   // Set the ball's position (using _position as is)
   circle.m_p.Set(_position.x, _position.y);
   circle.m_radius = static_cast<float>(_radius);

   // Create a Box2D polygon shape for the brick.
   cv::Rect2f brickRect = obj.get_size();
   b2PolygonShape polygon;
   // Compute half–dimensions.
   float halfWidth = brickRect.width / 2.0f;
   float halfHeight = brickRect.height / 2.0f;
   // Compute the center of the brick rectangle.
   float centerX = brickRect.x + halfWidth;
   float centerY = brickRect.y + halfHeight;
   // Set the polygon as a box: note that Box2D's SetAsBox uses half–width and half–height.
   polygon.SetAsBox(halfWidth, halfHeight, b2Vec2(centerX, centerY), 0.0f);

   // Create identity transforms for both shapes.
   b2Transform xf;
   xf.SetIdentity();

   // Use Box2D's b2TestOverlap to determine if the two shapes overlap.
   bool overlap = b2TestOverlap(&circle, 0, &polygon, 0, xf, xf);

   if (overlap) {
      // Basic bounce: simply invert the velocity.
      _velocity = -_velocity;
      return true;
      }
   return false;
   }

bool CBall::collide_paddle(CBBObjects& obj)
   {
   // 1. Create a Box2D circle shape for the ball.
   b2CircleShape circle;
   circle.m_p.Set(_position.x, _position.y);       // Ball center (in "pixel" coordinates)
   circle.m_radius = static_cast<float>(_radius);  // Ball radius

   // 2. Create a Box2D polygon shape for the paddle (assumed rectangular).
   cv::Rect2f paddleRect = obj.get_size();         // e.g. x,y,w,h
   b2PolygonShape polygon;
   float halfWidth = paddleRect.width / 2.0f;
   float halfHeight = paddleRect.height / 2.0f;
   float centerX = paddleRect.x + halfWidth;
   float centerY = paddleRect.y + halfHeight;

   // SetAsBox uses half-dimensions and a center point.
   polygon.SetAsBox(halfWidth, halfHeight, b2Vec2(centerX, centerY), 0.0f);

   // 3. Identity transforms (since we've embedded coordinates directly).
   b2Transform xf;
   xf.SetIdentity();

   // 4. Test overlap with b2TestOverlap.
   bool overlap = b2TestOverlap(&circle, 0, &polygon, 0, xf, xf);
   if (overlap)
      {
      // A collision occurred, so bounce the ball.

      // Convert current velocity to a Box2D vector.
      b2Vec2 ballVel(_velocity.x, _velocity.y);

      // Convert paddle velocity to a Box2D vector.
      b2Vec2 paddleVel(obj.get_vel().x, obj.get_vel().y);

      // Invert the vertical (y) component to bounce upward.
      ballVel.y = -ballVel.y;

      // If the paddle velocity is basically zero, slow the ball's x velocity.
      // Adjust 0.8f for how much to slow (80% of current).
      if (paddleVel.LengthSquared() < 1e-6f) // or pick a threshold you like
         {
         ballVel.x *= 0.8f;
         }
      else
         {
         // If the paddle is moving, add a fraction of the paddle's velocity for a "push" effect.
         // Adjust 0.3f to change how strongly the paddle affects the ball horizontally.
         ballVel += 0.3f * paddleVel;
         }

      // Update the ball's velocity.
      _velocity = cv::Point2f(ballVel.x, ballVel.y);

      return true;  // Indicate collision happened.
      }

   return false;     // No collision.
   }

bool CBall::collide_wall(cv::Size size,cv::Point2f paddle_position) {
   bool hitBottom = false;

   // Check left wall
   if (_position.x - _radius < 0) {
      _position.x = _radius;             // reposition inside the screen
      _velocity.x = -_velocity.x;          // bounce horizontally
      }

   // Check right wall
   if (_position.x + _radius > size.width) {
      _position.x = size.width - _radius;  // reposition inside the screen
      _velocity.x = -_velocity.x;          // bounce horizontally
      }

   // Check top wall
   if (_position.y - _radius < 0) {
      _position.y = _radius;               // reposition inside the screen
      _velocity.y = -_velocity.y;          // bounce vertically
      }

   // Check bottom wall
   if (_position.y + _radius >= size.height) {
      hitBottom = true; // indicate collision with bottom
      _position = cv::Point2f(paddle_position.x, paddle_position.y - (PADDLE_HEIGHT / 2) - _radius - 2);
      _velocity = cv::Point2f(0, 0);
      // Optionally, you might reposition or adjust velocity here,
      // but typically a bottom collision is treated as "game over" or life lost.
      }

   return hitBottom;
   }
///////////////////////////////////////////////////////////////////////////
void CBall::draw(cv::Mat& im) {
   cv::circle(im, _position, _radius, cv::Scalar(255, 0, 0), cv::FILLED);
   }
