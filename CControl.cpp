#include "stdafx.h"
#include "CControl.h"
#include "Serial.h"
#include "Windows.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <regex>
#include <unordered_map>

#define MAX_ANALOG 4096.0
#define JOYSTICK_X 2
#define JOYSTICK_Y 26
#define RGBLED_RED_PIN 39
#define SWITCH_2 32
#define COMM_MAX 10
#define SERVO_PIN 0

enum { DIGITAL = 0, ANALOG, SERVO };

using namespace std;

CControl::CControl() {
   }

CControl::~CControl() {

   }

void CControl::auto_connect() {
   while (true) {
      for (int comm_index = 0; comm_index <= COMM_MAX; comm_index++) {
         init_com(comm_index);
         if (_com.is_open()) {
            cout << "Device found" << endl;
            return;
            }
         cout << "Device not found" << endl;
         }
      }
   }

void CControl::init_com(int comport) {
   string port = "COM" + std::to_string(comport);
   _com.open(port);
   }

bool CControl::get_data(int type, int channel, int& result) {
   _com.flush();
   std::string tx_str = "G " + to_string(type) + " " + to_string(channel) + "\n";
   std::string rx_str;

   // temporary storage
   char buff[2];

   _com.write(tx_str.c_str(), tx_str.length());

   rx_str = "";
   // start timeout count
   double start_time = cv::getTickCount();

   buff[0] = 0;
   // Read 1 byte and if an End Of Line then exit loop
   // Timeout after 1 second, if debugging step by step this will cause you to exit the loop
   
   bool bad_data = true;
   while (buff[0] != '\n' && (cv::getTickCount() - start_time) / cv::getTickFrequency() < 1.0)
      {
      if (_com.read(buff, 1) > 0)
         {
         rx_str = rx_str + buff[0];
         if (buff[0] != '\n') {
            bad_data = false;
            }
         }
      }
   if (bad_data) {
      auto_connect();
      }

   if (rx_str.empty()) {
      return false;
      }

   size_t last_space = rx_str.find_last_of(' ');

   // Extract the substring after the last space
   string number_str = rx_str.substr(last_space + 1);
   if (number_str.empty()) {
      return false;
      }

   // Convert the substring to an integer
   int number = std::stoi(number_str);
   result = number;
   }

bool CControl::set_data(int type, int channel, int val) {
   std::string tx_str = "S " + to_string(type) + " " + to_string(channel) + " " + to_string(val) + "\n";
   _com.write(tx_str.c_str(), tx_str.length());
   Sleep(10);
   _com.flush();
   return true;
   }

void CControl::test_analog() {

   cv::Mat frame = cv::imread("Chatgpt.jpg");
   if (frame.empty()) {
      cv::Mat frame = cv::Mat::zeros(300, 300, CV_8UC3); // Black placeholder image
      }
   // Create a display window
   cv::namedWindow("Press 'q' to Exit", cv::WINDOW_AUTOSIZE);
   
   while (true) {
      float joystick_x_percent = get_analog(ANALOG, JOYSTICK_X);
      float joystick_y_percent = get_analog(ANALOG, JOYSTICK_Y);
      int joystick_x_value;
      int joystick_y_value;
      get_data(ANALOG, JOYSTICK_X, joystick_x_value);
      get_data(ANALOG, JOYSTICK_Y, joystick_y_value);
      cout << "ANALOG TEST: CH" << JOYSTICK_X << " X= " << joystick_x_value << "(";
      cout << fixed << setprecision(1) << joystick_x_percent << " %) CH";
      cout << JOYSTICK_Y << " Y= " << joystick_y_value << "(";
      cout << fixed << setprecision(1) << joystick_y_percent << " %)" << endl;
      cv::imshow("Press 'q' to Exit", frame);

      // Wait for 1 ms and capture the key press
      int key = cv::waitKey(1);

      // Check if the key matches the exit condition
      if (key == 'q' || key == 27) { // 'q' key or ESC (27)
         cv::destroyAllWindows();
         break; // Exit the loop
         }
      }
   }

void CControl::test_digital() {
   cv::Mat frame = cv::imread("Chatgpt.jpg");
   if (frame.empty()) {
      cv::Mat frame = cv::Mat::zeros(300, 300, CV_8UC3); // Black placeholder image
      }
   // Create a display window
   cv::namedWindow("Press 'q' to Exit", cv::WINDOW_AUTOSIZE);
   int LED_state = 0;
   while (true) {
      get_data(DIGITAL, SWITCH_2, LED_state);
      if (LED_state == 0) {
         LED_state = 1;
         }
      else {
         LED_state = 0;
         }
      cout << "DIGITAL TEST: CH" << SWITCH_2 << " = " << LED_state << endl;
      set_data(DIGITAL, RGBLED_RED_PIN, LED_state);
      cv::imshow("Press 'q' to Exit", frame);

      // Wait for 1 ms and capture the key press
      int key = cv::waitKey(1);

      // Check if the key matches the exit condition
      if (key == 'q' || key == 27) { // 'q' key or ESC (27)
         cv::destroyAllWindows();
         break; // Exit the loop
         }
      }
   }

void CControl::test_servo() {
   int Servo_present_position;
   cv::Mat frame = cv::imread("Chatgpt.jpg");
   if (frame.empty()) {
      cv::Mat frame = cv::Mat::zeros(300, 300, CV_8UC3); // Black placeholder image
      }
   // Create a display window
   cv::namedWindow("Press 'q' to Exit", cv::WINDOW_AUTOSIZE);
   while (true) {
      for (int Servo_position = 0; Servo_position <= 175; Servo_position += 5) {
         set_data(SERVO, SERVO_PIN, Servo_position);
         get_data(SERVO, SERVO_PIN, Servo_present_position);
         cout << "SERVO TEST: CH" << SERVO_PIN << " = " << Servo_present_position << endl;
         }
      for (int Servo_position = 175; Servo_position >= 0; Servo_position -= 5) {
         set_data(SERVO, SERVO_PIN, Servo_position);
         get_data(SERVO, SERVO_PIN, Servo_present_position);
         cout << "SERVO TEST: CH" << SERVO_PIN << " = " << Servo_present_position << endl;
         }

      if (!_com.is_open()) {
         auto_connect();
         }

      cv::imshow("Press 'q' to Exit", frame);

      // Wait for 1 ms and capture the key press
      int key = cv::waitKey(1);

      // Check if the key matches the exit condition
      if (key == 'q' || key == 27) { // 'q' key or ESC (27)
         cv::destroyAllWindows();
         break; // Exit the loop
         }
      }
   }

void CControl::test_button() {
   bool flag;
   int count = 0;
   int switch_state;
   cv::Mat frame = cv::imread("Chatgpt.jpg");
   if (frame.empty()) {
      cv::Mat frame = cv::Mat::zeros(300, 300, CV_8UC3); // Black placeholder image
      }
   // Create a display window
   cv::namedWindow("Press 'q' to Exit", cv::WINDOW_AUTOSIZE);

   cout << "BUTTON TEST: CH" << SWITCH_2 << " = 0" << endl;
   while (true) {
      if (get_button(SWITCH_2, switch_state)) {
         count++;
         cout << "BUTTON TEST: CH" << SWITCH_2 << " = " << count << endl;
         do {
            get_data(DIGITAL, SWITCH_2, switch_state);
            } while (switch_state == 0);
         }

      cv::imshow("Press 'q' to Exit", frame);

      // Wait for 1 ms and capture the key press
      int key = cv::waitKey(1);

      // Check if the key matches the exit condition
      if (key == 'q' || key == 27) { // 'q' key or ESC (27)
         cv::destroyAllWindows();
         break; // Exit the loop
         }
      }
   }

float CControl::get_analog(int type, int channel) {
   int analog_result;
   get_data(type, channel, analog_result);
   float analog_percentage;
   analog_percentage = analog_result * 100 / MAX_ANALOG;
   return analog_percentage;
   }

bool CControl::get_button(int channel, int& result) {
   ///////////////// ChatGPT
   static std::unordered_map<int, bool> button_states;
   static std::unordered_map<int, double> last_press_time;
   double current_time = cv::getTickCount() / cv::getTickFrequency();
   //////////////////
   int current_state;
   get_data(DIGITAL, channel, current_state);
   if (current_state == 0 && !button_states[channel] &&
      (current_time - last_press_time[channel]) > 0.02) {

      result = 1;
      button_states[channel] = true;
      last_press_time[channel] = current_time;
      return true;
      }
   if (current_state == 1) {
      button_states[channel] = false;
      }
   return false;
   result = 0;
   }

void CControl::print_menu() {
   cout << "\n****************************************";
   cout << "\nELEX4618 Embedded Control System, by Mantaj Chauhan";
   cout << "\n****************************************";
   cout << "\n(A)nalog test";
   cout << "\n(D)igital test";
   cout << "\n(B)utton test";
   cout << "\n(S)ervo test";
   cout << "\n(Q)uit\n";
   }

string CControl::valid_char() {
   string user_input;
   while (1) {
      cout << "CMD> ";
      cin >> ws;
      getline(cin, user_input);
      if (regex_match(user_input, regex("^.$"))) {
         return user_input;
         }
      cout << "Invalid input\n";
      }
   }

bool CControl::get_button_hold(int channel, int& result) {
   static std::unordered_map<int, double> last_hold_time;
   double current_time = cv::getTickCount() / cv::getTickFrequency();

   int current_state;
   get_data(DIGITAL, channel, current_state);

   // If pressed
   if (current_state == 0) {
      // Only consider the button "held" if it has been pressed for at least 0.02 seconds.
      if ((current_time - last_hold_time[channel]) > 0.02) {
         result = 1;
         // Update the time each frame if desired,
         // or only update once on the initial press.
         last_hold_time[channel] = current_time;
         return true;
         }
      else {
         result = 0;
         return false;
         }
      }
   else {
      // Reset the timer if button is released.
      last_hold_time[channel] = current_time;
      result = 0;
      return false;
      }
   }