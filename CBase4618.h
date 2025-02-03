/**
 * @file CBase4618.h
 * @brief Declaration of the CBase4618 class, providing a base interface for GPIO, update, and draw functionalities.
 */

#pragma once

#include "stdafx.h"
#include "CControl.h"

 /**
  * @class CBase4618
  * @brief A base class for managing GPIO, updates, and drawing in derived classes.
  *
  * This class serves as an abstract base class with virtual methods
  * that must be implemented by derived classes. It also provides
  * utility functions to manage the canvas, control, and exit state.
  */
class CBase4618 {
private:
   /**
    * @brief Instance of CControl to manage GPIO and control operations.
    */
   CControl _control;

   /**
    * @brief Canvas used for drawing operations.
    */
   cv::Mat _canvas;

   /**
    * @brief Flag to indicate exit state.
    */
   bool _exit;

public:
   /**
    * @brief Default constructor for the CBase4618 class.
    */
   CBase4618();

   /**
    * @brief Destructor for the CBase4618 class.
    */
   ~CBase4618();

   /**
    * @brief Pure virtual method for GPIO operations.
    *
    * Must be implemented by derived classes.
    */
   virtual void gpio() = 0;

   /**
    * @brief Pure virtual method for updating application state.
    *
    * Must be implemented by derived classes.
    */
   virtual void update() = 0;

   /**
    * @brief Pure virtual method for drawing on the canvas.
    *
    * Must be implemented by derived classes.
    */
   virtual void draw() = 0;

   /**
    * @brief Runs the main execution loop.
    *
    * This method handles the program's main execution cycle,
    * including GPIO, update, and draw steps.
    */
   void run();

   /**
    * @brief Gets the control object.
    * @return A reference to the CControl object.
    */
   CControl& getControl();

   /**
    * @brief Sets the control object.
    * @param control A constant reference to the CControl object to set.
    */
   void setControl(const CControl& control);

   /**
    * @brief Gets the canvas for drawing operations.
    * @return A reference to the cv::Mat canvas object.
    */
   cv::Mat& getCanvas();

   /**
    * @brief Sets the canvas for drawing operations.
    * @param canvas A constant reference to the cv::Mat canvas object to set.
    */
   void setCanvas(const cv::Mat& canvas);

   /**
    * @brief Gets the exit flag.
    * @return True if exit is set, false otherwise.
    */
   bool getExit();

   /**
    * @brief Sets the exit flag.
    * @param exit A constant boolean reference to set the exit flag.
    */
   void setExit(const bool& exit);
   };
