/*
 *  Copyright (C) 2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 *
 *  Originally taken from pvr.zattoo (https://github.com/rbuehlma/pvr.zattoo)
 */

#pragma once

#include <string>

class Utils
{
public:
  static time_t StringToTime(const std::string& timeString);
  static int StringToInt(const std::string& str, int defaultValue);
  static int Hash(const std::string& str);
  static std::string CreateUUID();
};
