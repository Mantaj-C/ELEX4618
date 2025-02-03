#pragma once

#include "Serial.h"
#include <string>

/**
 * @class CControl
 * @brief A class to manage communication and control functionality using Serial.
 */
class CControl {
private:
   /**
   * @brief serial communication object
   * Serial communication object for interfacing with hardware.
   */
   Serial _com;

public:
   /**
    * @brief Constructor for CControl.
    * Initializes the class object.
    */
   CControl();

   /**
    * @brief Destructor for CControl.
    * Cleans up resources associated with the class object.
    */
   ~CControl();

   /**
    * @brief Initializes the serial communication.
    *
    * @param comport The COM port number to establish communication.
    */
   void init_com(int comport);

   /**
    * @brief Retrieves data from the device.
    *
    * @param type The type of data to retrieve (Digital = 0, Analog = 1, Servo = 2).
    * @param channel The channel/pin to retrieve data from.
    * @param result Reference to store the retrieved data.
    * @return True if data retrieval was successful, false otherwise.
    */
   bool get_data(int type, int channel, int& result);

   /**
    * @brief Sends data to the device.
    *
    * @param type The type of data to set (Digital = 0, Analog = 1, Servo = 2).
    * @param channel The channel/pin to send data to.
    * @param val The value to set on the specified channel/pin.
    * @return True if data was sent successfully, false otherwise.
    */
   bool set_data(int type, int channel, int val);

   /**
    * @brief Tests digital inputs and outputs.
    * A function to test digital buttons and LEDs.
    */
   void test_digital();

   /**
    * @brief Tests analog joystick movements.
    * A function to test analog joystick movements.
    */
   void test_analog();

   /**
    * @brief Retrieves an analog value from a channel/pin.
    *
    * @param type The type of analog data to retrieve.
    * @param channel The channel/pin to retrieve the analog value from.
    * @return The analog value as a float percentage.
    */
   float get_analog(int type, int channel);

   /**
   * @brief Tests button functionality by continuously reading its state.
   *
   * This function is useful for verifying that the button is working correctly
   * and to observe how it behaves under different conditions.
   */
   void test_button();

   /**
    * @brief Retrieves the current state of a button and applies software debouncing.
    *
    * Reads the state of the button connected to the specified channel, ensuring
    * that bouncing caused by mechanical switches does not produce erroneous readings.
    *
    * @param channel The channel number associated with the button.
    * @param result A reference to store the debounced state of the button:
    *               - `1` for pressed
    *               - `0` for not pressed
    * @param flag A reference to store the previous state of the button:
    *               - `true` for pressed
    *               - `0` for not pressed
    *
    * @return True if the button state was successfully retrieved, false otherwise.
    */
   bool get_button(int channel, int& result);

   /**
    * @brief Tests servo motor functionality.
    * A function to sweep through the range of the servo connected to the system.
    */
   void test_servo();

   /**
    * @brief Validates single-character input from the user.
    *
    * @return A valid single character as a string.
    */
   std::string valid_char();

   /**
    * @brief Prints a control menu for the user.
    * Displays a list of available commands and options.
    */
   void print_menu();

   /**
    * @brief Automatically connects to the first available COM port.
    * Searches for available COM ports and establishes a connection.
    */
   void auto_connect();
   };
