/*
 *  Copyright (C) 2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 *
 *  Originally taken from pvr.zattoo (https://github.com/rbuehlma/pvr.zattoo)
 */

#include "Utils.h"

#include <cstdio>
#include <ctime>
#include <exception>
#include <random>
#include <string>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define timegm _mkgmtime
#endif

time_t Utils::StringToTime(const std::string& timeString)
{
  // expected timeString "2019-01-20T15:40:00+0100"
  struct tm tm = {};

  int year, month, day, h, m, s, tzh, tzm;
  if (std::sscanf(timeString.c_str(), "%d-%d-%dT%d:%d:%d%d", &year, &month, &day, &h, &m, &s,
                  &tzh) < 7)
  {
    tzh = 0;
  }
  tzm = tzh % 100;
  tzh = tzh / 100;

  tm.tm_year = year - 1900;
  tm.tm_mon = month - 1;
  tm.tm_mday = day;
  tm.tm_hour = h - tzh;
  tm.tm_min = m - tzm;
  tm.tm_sec = s;

  time_t ret = timegm(&tm);
  return ret;
}

int Utils::StringToInt(const std::string& str, int defaultValue)
{
  try
  {
    return std::stoi(str);
  }
  catch (std::exception& e)
  {
    return defaultValue;
  }
}

int Utils::Hash(const std::string& str)
{
  const char* s = str.c_str();

  int hash = 0;
  while (*s)
    hash = ((hash << 5) + hash) + *s++; // hash * 33 + c

  return std::abs(hash);
}

std::string Utils::CreateUUID()
{
  // https://stackoverflow.com/questions/24365331/how-can-i-generate-uuid-in-c-without-using-boost-library
  static std::random_device dev;
  static std::mt19937 rng(dev());

  std::uniform_int_distribution<int> dist(0, 15);

  const char* v = "0123456789abcdef";
  const bool dash[] = {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0};

  std::string res;
  for (int i = 0; i < 16; i++)
  {
    if (dash[i])
      res += "-";
    res += v[dist(rng)];
    res += v[dist(rng)];
  }
  return res;
}
