////////////////////////////////////////////////////////////////
// ELEX 4618 Template project for BCIT
// Created Oct 5, 2016 by Craig Hennessey
// NOTE: You must download the 'opencv.zip' file from D2L
// NOTE: Unzip and place in the parent folder before your lab folders
// i.e. you have \4618\opencv and then \4618\Labs
////////////////////////////////////////////////////////////////
#include "stdafx.h"

// Add simple GUI elements
#define CVUI_DISABLE_COMPILATION_NOTICES
#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <regex>

#include "Client.h"
#include "Server.h"
#include "CGrocery.h"
#include "CControl.h"
#include "CBase4618.h"
#include "CSnakeGame.h"
#include "CSnakeGameV2.h"
#include "CGameObject.h"
#include "CAsteroidGame.h"


// Must include Windows.h after Winsock2.h, so Serial must be included after Client/Server
#include "Serial.h" 

#define CANVAS_NAME "Display Image"
#define SNAKEGAME_CANVAS_WIDTH 1000
#define SNAKEGAME_CANVAS_HEIGHT 800

////////////////////////////////////////////////////////////////
// Can be used as a replacement for cv::waitKey() to display cv::imshow() images, Windows Only
////////////////////////////////////////////////////////////////
void process_msg()
{
  MSG msg;
  while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
  {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }
}

////////////////////////////////////////////////////////////////
// Generate ARUCO markers
////////////////////////////////////////////////////////////////
void generate_marks()
{
  std::string str;
  cv::Mat im;
  int mark_size = 250;

  cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
  
  for (int marker_id = 0; marker_id < 250; marker_id++)
  {
    cv::aruco::generateImageMarker(dictionary, marker_id, mark_size, im, 1);
    str = "mark" + std::to_string(marker_id) + ".png";
    cv::imwrite(str, im);
  }
}

////////////////////////////////////////////////////////////////
// Serial Communication
////////////////////////////////////////////////////////////////
void test_com()
{
  // Comport class (change port to your MSP device port)
  Serial com;
  com.open("COM3");

  // TX and RX strings
  std::string tx_str = "G 1 1\n";
  std::string rx_str;

  // temporary storage
  char buff[2];
  do
  {
    // Send TX string
		com.write(tx_str.c_str(), tx_str.length());
    Sleep(10); // wait for ADC conversion, etc. May not be needed?
  
    rx_str = "";
    // start timeout count
    double start_time = cv::getTickCount();

    buff[0] = 0;
		// Read 1 byte and if an End Of Line then exit loop
    // Timeout after 1 second, if debugging step by step this will cause you to exit the loop
    while (buff[0] != '\n' && (cv::getTickCount() - start_time) / cv::getTickFrequency() < 1.0)
    {
      if (com.read(buff, 1) > 0)
      {
        rx_str = rx_str + buff[0];
      }
    }

    printf ("\nRX: %s", rx_str.c_str());
    cv::waitKey(1);
  } 
  while (1);
}

////////////////////////////////////////////////////////////////
// Display Image on screen
////////////////////////////////////////////////////////////////
void do_image()
{
  cv::Mat im;

  // initialize GUI system
  cvui::init(CANVAS_NAME);
  cv::Point gui_position;

  // Load test image
  im = cv::imread("BCIT.jpg");

  // Seed random number generator with time
  srand(time(0));

  // Draw 500 circles and dots on the image
  for (int i = 0; i < 500; i++)
  {
    gui_position = cv::Point(10, 10);
    cvui::window(im, gui_position.x, gui_position.y, 200, 40, "Image Test");
    gui_position += cv::Point(5, 25);
    cvui::text(im, gui_position.x, gui_position.y, "Number of Circles: " + std::to_string(i));

    float radius = 50 * rand() / RAND_MAX;
    cv::Point center = cv::Point(im.size().width*rand() / RAND_MAX, im.size().height*rand() / RAND_MAX);
    
    cv::circle(im, center, radius, cv::Scalar(200, 200, 200), 1, cv::LINE_AA);
    
    im.at<char>(i,i) = 255;
    
    // Update the CVUI GUI system
    cvui::update();

    // Show image and delay (all display image to update)
    cv::imshow(CANVAS_NAME, im);
    cv::waitKey(1);
  }
}

////////////////////////////////////////////////////////////////
// Display Video on screen
////////////////////////////////////////////////////////////////
void do_video()
{
  cv::VideoCapture vid;

  vid.open(0, cv::CAP_DSHOW);
    
  bool do_canny = true;
  bool do_aruco = false;
  int canny_thresh = 30;
  bool do_exit = false;

  cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
  cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
  cv::aruco::ArucoDetector detector(dictionary, detectorParams);

  // initialize GUI system
  cvui::init(CANVAS_NAME);
  cv::Point gui_position;
  
  std::vector<cv::Scalar> color_vec;
  color_vec.push_back(cv::Scalar(255, 255, 255));
  color_vec.push_back(cv::Scalar(255, 0, 0));
  color_vec.push_back(cv::Scalar(0, 255, 0));
  color_vec.push_back(cv::Scalar(0, 0, 255));
  int color_index = 0;
  
  if (vid.isOpened() == TRUE)
  {
    do
    {
      cv::Mat frame, edges;

      // Capture video frame
      vid >> frame;

      // Make sure video frame exists
      if (frame.empty() == false)
      { 
        // ARUCO marker tracking
        if (do_aruco == true)
        {
          std::vector<int> ids;
          std::vector<std::vector<cv::Point2f>> corners;
          detector.detectMarkers(frame, corners, ids);

          if (ids.size() > 0)
          {
            cv::aruco::drawDetectedMarkers(frame, corners, ids);
          }
        }

        // Canny edge detection
        if (do_canny == true)
        {
          cv::cvtColor(frame, edges, cv::COLOR_BGR2GRAY);
          cv::GaussianBlur(edges, edges, cv::Size(7, 7), 1.5, 1.5);
          cv::Canny(edges, edges, 0, canny_thresh, 3);
          cv::add(frame, color_vec.at(color_index), frame, edges);
        }

        // GUI Menu
        gui_position = cv::Point(10, 10);
        cvui::window(frame, gui_position.x, gui_position.y, 200, 190, "Video Test");
        gui_position += cv::Point(5, 25);
        cvui::checkbox(frame, gui_position.x, gui_position.y, "Canny Filter", &do_canny);
        gui_position += cv::Point(0, 25);
        cvui::checkbox(frame, gui_position.x, gui_position.y, "ArUco", &do_aruco);
        gui_position += cv::Point(0, 25);
        cvui::text(frame, gui_position.x, gui_position.y, "Canny Threshold");
        gui_position += cv::Point(0, 15);
        cvui::trackbar(frame, gui_position.x, gui_position.y, 180, &canny_thresh, 5, 120);
        gui_position += cv::Point(0, 50);
        if (cvui::button(frame, gui_position.x, gui_position.y, 100, 30, "Colour Switch"))
        {
          color_index++;
          if (color_index >= color_vec.size()) { color_index = 0; }
        }
        gui_position += cv::Point(120, 0);
        if (cvui::button(frame, gui_position.x, gui_position.y, 50, 30, "Exit"))
        {
          do_exit = true;
        }

        // Update the CVUI GUI system
        cvui::update();
        cv::imshow(CANVAS_NAME, frame);
      }
    }
    while (cv::waitKey(1) != 'q' && do_exit == false);
  }      
}		

////////////////////////////////////////////////////////////////
// Demo client server communication
////////////////////////////////////////////////////////////////
void serverthread(CServer* server)
{
  // Start server
  server->start(4618);
}

void do_clientserver()
{
  char inputchar = 0;
  std::vector<std::string> cmds;

  cv::VideoCapture vid;
  CServer server;

  // Start server thread
  std::thread t(&serverthread, &server);
  t.detach();

  vid.open(0);

  while (inputchar != 'q')
  {
    inputchar = cv::waitKey(100);

    server.get_cmd(cmds);
    if (cmds.size() > 0)
    {
      // Process different commands received by the server
      for (int i = 0; i < cmds.size(); i++)
      {
        if (cmds.at(i) == "im")
        {
          std::cout << "\nServer Rx: " << cmds.at(i);

          std::string reply = "Hi there from Server";
          server.send_string(reply);
        }
        else
        {
          std::cout << "\nServer Rx: " << cmds.at(i);

          std::string reply = "Got some other message";
          server.send_string(reply);
        }
      }
    }

		// Update server image with the latest camera image
		if (vid.isOpened() == true)
		{
			cv::Mat frame;
			vid >> frame;
			if (frame.empty() == false)
			{
				imshow("Server Image", frame);
				process_msg();
				server.set_txim(frame);
			}
		}
  };

  server.stop();
  
  Sleep(100);
}

////////////////////////////////////////////////////////////////
// Lab 1
////////////////////////////////////////////////////////////////
struct Products {
    char product_names[100][10];
    float price[100];
    int quantity[100];
    int number_of_products = 0;
    float total_value[100];
};
Products products;
void print_menu2();
void add_product(Products& products);
void edit_product(Products& products);
void print_products(Products& products);
void lab1()
{
    while (1) {
        char userin;
        print_menu2();
        std::cout << "CMD>";
        std::cin >> userin;
        
        if (userin == 'A' || userin == 'a') {
            add_product(products);
        }
        else if (userin == 'E' || userin == 'e') {
            edit_product(products);
        }
        else if (userin == 'P' || userin == 'p') {
            print_products(products);
        }
        else if (userin == 'Q' || userin == 'q'){
            break;
        }
    }
}
void print_menu2() {
    std::cout << "\n****************************************";
        std::cout << "\nELEX4618 Grocery System, by Mantaj Chauhan";
        std::cout << "\n****************************************";
        std::cout << "\n(A)dd product";
        std::cout << "\n(E)dit product";
        std::cout << "\n(P)rint products";
        std::cout << "\n(Q)uit\n";
}
void add_product(Products& products) {
    std::cout << "\nadding product\n";
    std::cout << "Product name: ";
    std::cin >> products.product_names[products.number_of_products];
    products.number_of_products = products.number_of_products + 1;
}
void edit_product(Products& products) {
   int product_select;
   std::cout << "\nProduct to Edit: ";
   std::cin >> product_select;
   std::cout << "Product name: " << products.product_names[product_select - 1];
   std::cout << "\nProduct price: ";
   std::cin >> products.price[product_select - 1];
   std::cout << "Product quantity: ";
   std::cin >> products.quantity[product_select - 1];
   products.total_value[product_select - 1] = products.price[product_select - 1] * products.quantity[product_select - 1];
}
void print_products(Products& products) {
   const int product_length = 13;
   const int price_length = 10;
   const int quantit_length = 14;

   std::cout << "\nProduct          Price      Quantity      Value\n";

   for (int i = 1; i <= products.number_of_products; ++i) {
      std::cout << "(" << i << ") ";
      std::cout << std::left << std::setw(product_length) << std::setfill(' ') << products.product_names[i - 1];
      std::cout << "$" << std::left << std::setw(price_length) << std::setfill(' ') << products.price[i - 1];
      std::cout << std::left << std::setw(quantit_length) << std::setfill(' ') << products.quantity[i - 1];
      std::cout << "$" << products.total_value[i-1];
      std::cout << "\n";
   }
   float final_value = 0;
   for (int j = 0; j < products.number_of_products; ++j) {
      final_value = final_value + products.total_value[j];
      if (j == products.number_of_products - 1)
         {
      std::cout << "\nTotal: $" << final_value << "\n";
         }
   }
}

////////////////////////////////////////////////////////////////
// Lab 2
////////////////////////////////////////////////////////////////
void lab2()
{
   CGrocery grocery;
   while (1) {
      std::string userin;
      grocery.print_menu3();
      userin = grocery.valid_char();

      if (userin == "A" || userin == "a") {
         grocery.add_product();
         }
      else if (userin == "E" || userin == "e") {
         grocery.edit_product();
         }
      else if (userin == "P" || userin == "p") {
         grocery.print_products();
         }
      else if (userin == "D" || userin == "d") {
         grocery.delete_product();
         }
      else if (userin == "s" || userin == "S") {
         grocery.save_products();
         }
      else if (userin == "L" || userin == "l") {
         grocery.load_products();
         }
      else if (userin == "Q" || userin == "q") {
         break;
         }
      else {
         std::cout << "Invalid input";
         }
      }
}

////////////////////////////////////////////////////////////////
// Lab 3
////////////////////////////////////////////////////////////////
void lab3()
{
   CControl control;
      control.auto_connect();
   while (1) {
      std::string userin;
      control.print_menu();
      userin = control.valid_char();
      if (userin == "A" || userin == "a") {
         control.test_analog();
         }
      else if (userin == "d" || userin == "D") {
         control.test_digital();
         }
      else if (userin == "b" || userin == "B") {
         control.test_button();
         }
      else if (userin == "s" || userin == "S") {
         control.test_servo();
         }
      else if (userin == "Q" || userin == "q") {
         break;
         }
      else {
         std::cout << "Invalid input";
         }
      }
}

////////////////////////////////////////////////////////////////
// Lab 4
////////////////////////////////////////////////////////////////
void lab4()
{
   CSnakeGame snakegame(cv::Size(SNAKEGAME_CANVAS_WIDTH,SNAKEGAME_CANVAS_HEIGHT));
   snakegame.run();
}

////////////////////////////////////////////////////////////////
// Lab 5
////////////////////////////////////////////////////////////////
void lab5()
{
   CSnakeGameV2 snakegame(cv::Size(SNAKEGAME_CANVAS_WIDTH, SNAKEGAME_CANVAS_HEIGHT));
   snakegame.runMT();
}

////////////////////////////////////////////////////////////////
// Lab 6
////////////////////////////////////////////////////////////////
void lab6()
{
   CAsteroidGame asteroidgame(cv::Size(SNAKEGAME_CANVAS_WIDTH, SNAKEGAME_CANVAS_HEIGHT));
   asteroidgame.runMT();
}

void print_menu()
{
	std::cout << "\n***********************************";
	std::cout << "\n* ELEX4618 Template Project";
	std::cout << "\n***********************************";
  std::cout << "\n(1) Lab 1 - User Input";
  std::cout << "\n(2) Lab 2 - Grocery Store";
  std::cout << "\n(3) Lab 3 - Embedded Control";
  std::cout << "\n(4) Lab 4 - Snake Game";
  std::cout << "\n(5) Lab 5 - Snake Game V2";
  std::cout << "\n(6) Lab 6 - Classic Arcade Game";
  std::cout << "\n(7) Lab 7 - Linux Port";
  std::cout << "\n(8) Lab 8 - Sorting";
  std::cout << "\n(9) Lab 9 - Sockets";
  std::cout << "\n(10) Test serial COM communication";
	std::cout << "\n(11) Show image manipulation";
	std::cout << "\n(12) Show video manipulation";
	std::cout << "\n(13) Test client/server communication";
	std::cout << "\n(0) Exit";
	std::cout << "\nCMD> ";
}

int main(int argc, char* argv[])
{
  // generate_marks();

	int cmd = -1;
	do
	{
		print_menu();
		std::cin >> cmd;
		switch (cmd)
		{
    case 1: lab1(); break;
    case 2: lab2(); break;
    case 3: lab3(); break;
    case 4: lab4(); break;
    case 5: lab5(); break;
    case 6: lab6(); break;
    case 10: test_com(); break;
		case 11: do_image(); break;
		case 12: do_video(); break;
    case 13: do_clientserver(); break;
		}
	} while (cmd != 0);
}
