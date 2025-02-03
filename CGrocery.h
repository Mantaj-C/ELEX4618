#pragma once
#include <iostream>
#include <string>
#include <regex>
#include "stdafx.h"
#include "CProduct.h"
using namespace std;

class CGrocery {
private:
   vector<CProduct> m_product_list;

public:
   CGrocery();
   void add_product();
   void edit_product();
   void print_products();
   void delete_product();
   void save_products();
   void load_products();
   void print_menu3();
   int valid_id();
   float valid_price();
   int valid_quantity();
   string valid_char();
   };
