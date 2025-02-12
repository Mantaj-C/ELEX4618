////////////////////////////////////////////////ChatGPT
// RainbowColors.h
#pragma once

#include <array>

// Namespace to hold our rainbow colors.
namespace RainbowColors {

   // Helper function to convert HSV (with h in degrees, s and v in [0,1])
   // to a cv::Scalar in BGR order (each channel in [0,255]).
   inline cv::Scalar HSVtoBGR(float h, float s, float v) {
      float c = v * s;
      float hPrime = h / 60.0f;
      float x = c * (1 - fabsf(fmod(hPrime, 2.0f) - 1));
      float r = 0.0f, g = 0.0f, b = 0.0f;

      if (0.0f <= hPrime && hPrime < 1.0f) {
         r = c; g = x; b = 0;
         }
      else if (1.0f <= hPrime && hPrime < 2.0f) {
         r = x; g = c; b = 0;
         }
      else if (2.0f <= hPrime && hPrime < 3.0f) {
         r = 0; g = c; b = x;
         }
      else if (3.0f <= hPrime && hPrime < 4.0f) {
         r = 0; g = x; b = c;
         }
      else if (4.0f <= hPrime && hPrime < 5.0f) {
         r = x; g = 0; b = c;
         }
      else if (5.0f <= hPrime && hPrime < 6.0f) {
         r = c; g = 0; b = x;
         }

      float m = v - c;
      r += m;
      g += m;
      b += m;

      // OpenCV uses BGR order.
      return cv::Scalar(b * 255, g * 255, r * 255);
      }

   // Returns a constant array of 100 rainbow shades.
   inline const std::array<cv::Scalar, 100>& getRainbowColors() {
      static std::array<cv::Scalar, 100> rainbowColors;
      static bool initialized = false;
      if (!initialized) {
         // Generate 100 colors spanning the hue range 0–360.
         for (int i = 0; i < 100; ++i) {
            // Linearly interpolate hue from 0 to 360 degrees.
            float hue = (360.0f * i) / 100.0f;
            rainbowColors[i] = HSVtoBGR(hue, 1.0f, 1.0f);
            }
         initialized = true;
         }
      return rainbowColors;
      }

   } // namespace RainbowColors

///////////////////////////////////////////////