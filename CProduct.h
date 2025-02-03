#pragma once
using namespace std;

class CProduct {
private:
   string m_product_list;
   float m_product_price;
   int m_product_quantity;
   float m_product_value;

public:
   CProduct(const string& name, float price, int quantity,float value);
   string get_product_name() const;
   float get_product_price() const;
   int get_product_quantity() const;
   float get_product_value() const;

   void set_product_name(const string& name);
   void set_product_price(float price);
   void set_product_quantity(int quantity);
   void set_product_value(float value);
   };
