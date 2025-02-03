#include <iostream>
#include <string>
#include <regex>
#include "stdafx.h"
#include "CGrocery.h"
#include "CProduct.h"
#include "tinyxml2.h"
using namespace std;
using namespace tinyxml2;

CGrocery::CGrocery() {
   }

void CGrocery::add_product() {
   string name;
   cout << "\nadding product\n";
   cout << "Product name: ";
   cin >> name;
   ///////////////////////////////from ChapGPT
   CProduct new_product(name, 0.0f, 0, 0.0f);
   m_product_list.push_back(new_product);
   //////////////////////////////////////
   }

void CGrocery::edit_product() {
   int product_select;
   if (m_product_list.empty()) {
      cout << "\nNo products available to edit." << endl;
      return;
      }
   else {
      product_select = valid_id();
      CProduct& product = m_product_list[product_select - 1];
      float user_price;
      int user_quantity;
      user_price = valid_price();
      user_quantity = valid_quantity();
      product.set_product_price(user_price);
      product.set_product_quantity(user_quantity);
      product.set_product_value(user_price * user_quantity);
      return;
      }
   }

void CGrocery::print_products() {
   int product_length = 3;
   int price_length = 9;
   int quantit_length = 9;
 
   for (int product_index = 0; product_index < m_product_list.size(); ++product_index) {
      const CProduct& product = m_product_list.at(product_index);
      string interm_name = product.get_product_name();

      if (interm_name.size() > product_length)
         {
         product_length = interm_name.size();
         }
      }

   cout << left << setw(product_length+7) << setfill(' ') << "\nProduct";
   cout << left << setw(price_length+3) << setfill(' ') << "Price";
   cout << left << setw(quantit_length+3) << setfill(' ') << "Quantity";
   cout << "Value\n";

   for (int product_index = 0; product_index < m_product_list.size(); ++product_index) {
      const CProduct& product = m_product_list.at(product_index);
      cout << "(" << product_index +1 << ") ";
      cout << left << setw(product_length+2) << setfill(' ') << product.get_product_name();
      cout << fixed << setprecision(2);
      cout << "$" << left << setw(price_length+2) << setfill(' ') << product.get_product_price();
      cout << left << setw(quantit_length+3) << setfill(' ') << product.get_product_quantity();
      cout << fixed << setprecision(2);
      cout << "$" << product.get_product_value() << endl;
      }

   float final_value = 0;
   for (int product_index = 0; product_index < m_product_list.size(); ++product_index) {
      const CProduct& product = m_product_list.at(product_index);
      final_value = final_value + product.get_product_value();
      if (product_index == m_product_list.size() - 1)
         {
         cout << fixed << setprecision(2);
         cout << "\nTotal: $" << final_value << endl;
         }
      }
   }

void CGrocery::delete_product() {
   int product_select;
   if (m_product_list.empty()) {
      cout << "\nNo products available to delete." << endl;
      return;
      }
   else {
      product_select = valid_id();
      CProduct& product = m_product_list[product_select - 1];
      m_product_list.erase(m_product_list.begin() + (product_select - 1));
      return;
      }
   }

void CGrocery::save_products() {
   string user_input;
   while (1) {
      cout << "Enter XLM file name (ex.Products.xml): ";
      cin >> ws;
      getline(cin, user_input);
      if (regex_match(user_input, regex(".*\\.xml$"))) {
         break;
         }
      cout << "Invalid file name\n";
      }
   
   XMLDocument file;

   XMLElement* root = file.NewElement("Products");
   file.InsertFirstChild(root);
   /////////////////////////////////////////////////  From ChatGPT
   for (const auto& product : m_product_list) {
      XMLElement* productElement = file.NewElement("Product");

      XMLElement* nameElement = file.NewElement("Name");
      nameElement->SetText(product.get_product_name().c_str());
      productElement->InsertEndChild(nameElement);

      XMLElement* priceElement = file.NewElement("Price");
      priceElement->SetText(product.get_product_price());
      productElement->InsertEndChild(priceElement);

      XMLElement* quantityElement = file.NewElement("Quantity");
      quantityElement->SetText(product.get_product_quantity());
      productElement->InsertEndChild(quantityElement);

      XMLElement* valueElement = file.NewElement("Value");
      valueElement->SetText(product.get_product_value());
      productElement->InsertEndChild(valueElement);

      root->InsertEndChild(productElement);
      }
   ////////////////////////////////////////////////////

   string fileName = user_input;
   XMLError error_check = file.SaveFile(fileName.c_str());
   if (error_check != XML_SUCCESS) {
      cerr << "Error saving file: " << file.ErrorIDToName(error_check) << endl;
      }
   else {
      cout << "Products saved to " << user_input << endl;
      }
   }

void CGrocery::load_products() {
   string user_input;
   while (1) {
      cout << "Enter XLM file name (ex.Products.xml): ";
      cin >> ws;
      getline(cin, user_input);
      if (regex_match(user_input, regex(".*\\.xml$"))) {
         break;
         }
      cout << "Invalid file name\n";
      }
   
   XMLDocument file;

   string fileName = user_input;
   XMLError error_check = file.LoadFile(fileName.c_str());
   if (error_check != XML_SUCCESS) {
      cerr << "Error loading file: " << user_input << endl;
      return;
      }

   XMLElement* root = file.FirstChildElement("Products");
   if (root == nullptr) {
      cerr << "Error no products.\n";
      return;
      }
   ///////////////////////////////////////////////////////// From ChatGPT
   m_product_list.clear();
   for (XMLElement* productElement = root->FirstChildElement("Product");
      productElement != nullptr;
      productElement = productElement->NextSiblingElement("Product")) {

      const char* name = productElement->FirstChildElement("Name")->GetText();
      float price = 0.0f;
      productElement->FirstChildElement("Price")->QueryFloatText(&price);

      int quantity = 0;
      productElement->FirstChildElement("Quantity")->QueryIntText(&quantity);

      float value = 0.0f;
      productElement->FirstChildElement("Value")->QueryFloatText(&value);

      m_product_list.emplace_back(name, price, quantity, value);
      }
   /////////////////////////////////////////////////////////////////////
   cout << "Products loaded from " << user_input << endl;
   }

void CGrocery::print_menu3() {
   cout << "\n****************************************";
   cout << "\nELEX4618 Grocery System, by Mantaj Chauhan";
   cout << "\n****************************************";
   cout << "\n(A)dd product";
   cout << "\n(E)dit product";
   cout << "\n(P)rint products";
   cout << "\n(D)elete products"; 
   cout << "\n(S)ave products";
   cout << "\n(L)oad products";
   cout << "\n(Q)uit\n";
   }

int CGrocery::valid_id() {
   string user_input;
   while (1) {
      cout << "\nEnter product id: ";
      cin >> ws;
      getline(cin, user_input);
      if (regex_match(user_input, regex("^\\d+$"))) {
         if (stoi(user_input) > 0 && stoi(user_input) <= m_product_list.size()) {
            return stoi(user_input);
            }
         }
      cout << "\nInvalid input enter an ID between 1-" << m_product_list.size();
      }
   }

float CGrocery::valid_price() {
   string user_input;
   while (1) {
      cout << "Enter price: ";
      cin >> ws;
      getline(cin, user_input);
      ///////////////////////////////////////////////////////////////////from ChatGPT
      if (regex_match(user_input, regex("^[0-9]+(\\.[0-9]{1,2})?$"))) {
         try {
            return stof(user_input);
            }
         catch (const out_of_range& e) {
            cout << "Input is too large for a float. Try again.\n";
            }
         }
      //////////////////////////////////////////////////////////////////////////////
      else {
         cout << "Invalid input\n";
         }
      }
   }

int CGrocery::valid_quantity() {
   string user_input;
   while (1) {
      cout << "Enter quantity: ";
      cin >> ws;
      getline(cin, user_input);
      if (regex_match(user_input, regex("^\\d+$"))) {
         return stoi(user_input);
         }
      cout << "\nInvalid input\n";
      }
   }

string CGrocery::valid_char() {
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

