#include <iostream>
#include <string>
#include "stdafx.h"
#include "CProduct.h"
#include "CGrocery.h"
using namespace std;

CProduct::CProduct(const string& name, float price, int quantity, float value)
   : m_product_list(name), m_product_price(price),
   m_product_quantity(quantity), m_product_value(value) {
   }

//GETTERS
string CProduct::get_product_name() const {
   return m_product_list;
   }

float CProduct::get_product_price() const {
   return m_product_price;
   }

int CProduct::get_product_quantity() const {
   return m_product_quantity;
   }

float CProduct::get_product_value() const {
   return m_product_value;
   }

//SETTERS
void CProduct::set_product_name(const string& name) {
   m_product_list = name;
   }

void CProduct::set_product_price(float price) {
   m_product_price = price;
   }

void CProduct::set_product_quantity(int quantity) {
   m_product_quantity = quantity;
   }

void CProduct::set_product_value(float value) {
   m_product_value = value;
   }