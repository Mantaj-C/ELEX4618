#pragma once

#include "stdafx.h"
#include "CBBObjects.h"

class CPaddle : public CBBObjects {
private:

public:
   CPaddle(cv::Size size);
   ~CPaddle();
   void move(int direction);
   };