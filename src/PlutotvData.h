/*
 *  Copyright (C) 2020 flubshi (https://github.com/flubshi)
 *  Copyright (C) 2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "Curl.h"
#include "kodi/addon-instance/PVR.h"
#include "rapidjson/document.h"

#include <vector>

/**
 * User Agent for HTTP Requests
 */
static const std::string PLUTOTV_USER_AGENT =
    "Mozilla/5.0 (Windows NT 6.2; rv:24.0) Gecko/20100101 Firefox/24.0";

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
  ADDON_STATUS SetSetting(const std::string& settingName,
                          const kodi::CSettingValue& settingValue) override;

  PVR_ERROR GetCapabilities(kodi::addon::PVRCapabilities& capabilities) override;
  PVR_ERROR GetBackendName(std::string& name) override;
  PVR_ERROR GetBackendVersion(std::string& version) override;

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

  rapidjson::Document m_epg_cache_document;
  time_t m_epg_cache_start = time_t(0);
  time_t m_epg_cache_end = time_t(0);;

  std::vector<PlutotvChannel> m_channels;
  bool m_bChannelsLoaded = false;

  void AddTimerType(std::vector<kodi::addon::PVRTimerType>& types, int idx, int attributes);

  std::string GetChannelStreamUrl(int uniqueId);
  std::string GetSettingsUUID(std::string setting);
  void SetStreamProperties(std::vector<kodi::addon::PVRStreamProperty>& properties,
                           const std::string& url,
                           bool realtime);

  std::string HttpGet(const std::string& url);
  std::string HttpDelete(const std::string& url, const std::string& postData);
  std::string HttpPost(const std::string& url, const std::string& postData);
  std::string HttpRequest(const std::string& action,
                          const std::string& url,
                          const std::string& postData);
  std::string HttpRequestToCurl(Curl& curl,
                                const std::string& action,
                                const std::string& url,
                                const std::string& postData,
                                int& statusCode);
  bool LoadChannelsData();
};
