#pragma once
/*
 *      Copyright (C) 2020 flubshi
 *      https://github.com/flubshi
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "Curl.h"
#include "kodi/addon-instance/PVR.h"
#include "p8-platform/os.h"
#include "rapidjson/document.h"

#include <vector>

/**
 * User Agent for HTTP Requests
 */
static const std::string PLUTOTV_USER_AGENT = "Mozilla/5.0 (Windows NT 6.2; rv:24.0) Gecko/20100101 Firefox/24.0";

class ATTRIBUTE_HIDDEN PlutotvData : public kodi::addon::CAddonBase,
                                   public kodi::addon::CInstancePVRClient
{
public:
  PlutotvData() = default;
  PlutotvData(const PlutotvData&) = delete;
  PlutotvData(PlutotvData&&) = delete;
  PlutotvData& operator=(const PlutotvData&) = delete;
  PlutotvData& operator=(PlutotvData&&) = delete;

  ADDON_STATUS Create() override;
  ADDON_STATUS GetStatus() override;
  ADDON_STATUS SetSetting(const std::string& settingName,
                          const kodi::CSettingValue& settingValue) override;

  PVR_ERROR GetCapabilities(kodi::addon::PVRCapabilities& capabilities) override;
  PVR_ERROR GetBackendName(std::string& name) override;
  PVR_ERROR GetBackendVersion(std::string& version) override;
  PVR_ERROR GetConnectionString(std::string& connection) override;

  PVR_ERROR GetChannelsAmount(int& amount) override;
  PVR_ERROR GetChannels(bool radio, kodi::addon::PVRChannelsResultSet& results) override;

  PVR_ERROR GetChannelGroupsAmount(int& amount) override;
  PVR_ERROR GetChannelGroups(bool radio, kodi::addon::PVRChannelGroupsResultSet& results) override;
  PVR_ERROR GetChannelGroupMembers(const kodi::addon::PVRChannelGroup& group,
                                   kodi::addon::PVRChannelGroupMembersResultSet& results) override;
  PVR_ERROR GetChannelStreamProperties(
      const kodi::addon::PVRChannel& channel,
      std::vector<kodi::addon::PVRStreamProperty>& properties) override;

  PVR_ERROR GetEPGForChannel(int channelUid,
                             time_t start,
                             time_t end,
                             kodi::addon::PVREPGTagsResultSet& results) override;


private:

  struct PlutotvChannel
  {
    int iUniqueId;
    std::string plutotvID;
    int iChannelNumber; //position
    std::string strChannelName;
    std::string strIconPath;
    std::string strStreamURL;
  };

  rapidjson::Document cache_document;
  std::string cache_url = "";


  ADDON_STATUS m_curStatus = ADDON_STATUS_OK;


  std::vector<PlutotvChannel> m_channels;

  void AddTimerType(std::vector<kodi::addon::PVRTimerType>& types, int idx, int attributes);

  std::string GetChannelStreamUrl(int uniqueId);
  std::string GetLicense(void);
  std::string GetSettingsUUID(std::string setting);
  void SetStreamProperties(std::vector<kodi::addon::PVRStreamProperty>& properties,
                           const std::string& url,
                           bool realtime);

  std::string HttpGet(const std::string& url);
  std::string HttpDelete(const std::string& url, const std::string& postData);
  std::string HttpPost(const std::string& url, const std::string& postData);
  std::string HttpRequest(const std::string& action, const std::string& url, const std::string& postData);
  std::string HttpRequestToCurl(
      Curl& curl, const std::string& action, const std::string& url, const std::string& postData, int& statusCode);
  bool LoadChannelData(void);
};
