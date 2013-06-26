#include "ValPos.h"
ValPos::~ValPos()
{
  if (value && type != STRINGTYPE)
    delete[] value;
}
