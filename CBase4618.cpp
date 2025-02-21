#include "stdafx.h"
#include "CBase4618.h"
#include "CControl.h"
#include <thread>

CBase4618::CBase4618() : _exit(false) {
   }

CBase4618::~CBase4618() {
   }

void CBase4618::run() {
   _control.auto_connect();
   while (true) {
      gpio();
      update();
      draw();
      if (cv::waitKey(1) == 'q' || _exit) {
         cv::destroyAllWindows();
         _exit = false;
         break;
         }
      }
   }
///////////////////////////////////////////////////ChatGPT
// In multithreaded mode, run gpio() and update() on separate threads,
// but call draw() in the main thread so that cv::imshow and cv::waitKey work reliably.
void CBase4618::runMT() {
   _control.auto_connect();

   // Spawn gpio and update on separate threads.
   std::thread gpio_thread(&CBase4618::gpio, this);
   std::thread update_thread(&CBase4618::update, this);

   // Run draw() in the main thread.
   draw();

   // After draw() returns (exit requested), wait for the other threads.
   gpio_thread.join();
   update_thread.join();
   }
////////////////////////////////////////////////

CControl& CBase4618::getControl() {
   return _control;
   }

void CBase4618::setControl(const CControl& control) {
   _control = control;
   }

cv::Mat& CBase4618::getCanvas() {
   return _canvas;
   }

void CBase4618::setCanvas(const cv::Mat& canvas) {
   _canvas = canvas.clone();
   }

bool CBase4618::getExit() {
   return _exit;
   }

void CBase4618::setExit(const bool& exit) {
   _exit = exit;
   }
