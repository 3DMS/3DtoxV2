/* 3DTox V2
 * Copyright (C) 2019 by Nicolas Rambaud
 *
 * This file is part of the 3DTox V2 firmware
 *
 * This file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License V3.0 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this piece of code.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "macros.h"
#include <stdint.h>

#define str(x) #x
#define xstr(x) str(x)
#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

  void crc16(uint16_t *crc, const void * const data, uint16_t cnt);

  // Convert uint8_t to string with 123 format
  char* i8tostr3(const uint8_t x);

  // Convert signed int to rj string with 123 or -12 format
  char* itostr3(const int x);

  // Convert unsigned int to lj string with 123 format
  char* itostr3left(const int xx);

  // Convert signed int to rj string with _123, -123, _-12, or __-1 format
  char* itostr4sign(const int x);

  // Convert unsigned float to string with 1.23 format
  char* ftostr12ns(const float &x);

  // Convert signed float to fixed-length string with 023.45 / -23.45 format
  char* ftostr52(const float &x);

  // Convert float to fixed-length string with +123.4 / -123.4 format
  char* ftostr41sign(const float &x);

  // Convert signed float to string (6 digit) with -1.234 / _0.000 / +1.234 format
  char* ftostr43sign(const float &x, char plus=' ');

  // Convert unsigned float to rj string with 12345 format
  char* ftostr5rj(const float &x);

  // Convert signed float to string with +1234.5 format
  char* ftostr51sign(const float &x);

  // Convert signed float to space-padded string with -_23.4_ format
  char* ftostr52sp(const float &x);

  // Convert signed float to string with +123.45 format
  char* ftostr52sign(const float &x);

  // Convert unsigned float to string with 1234.56 format omitting trailing zeros
  char* ftostr62rj(const float &x);

  // Convert float to rj string with 123 or -12 format
  FORCE_INLINE char* ftostr3(const float &x) { return itostr3(int(x + (x < 0 ? -0.5f : 0.5f))); }

  // Convert float to rj string with 1234, _123, 12.3, _1.2, -123, _-12, or -1.2 format
  char* ftostr4sign(const float &fx);

#endif // __UTILITY_H__
