/*
 *  Copyright (C) 2020 flubshi (https://github.com/flubshi)
 *  Copyright (C) 2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "PlutotvData.h"

#include "Utils.h"
#include "kodi/General.h"
#include "rapidjson/document.h"

#include <algorithm>
#include <ctime>
#include <regex>

// BEGIN CURL helpers from zattoo addon:
std::string PlutotvData::HttpGet(const std::string& url)
{
  return HttpRequest("GET", url, "");
}

std::string PlutotvData::HttpDelete(const std::string& url, const std::string& postData)
{
  return HttpRequest("DELETE", url, postData);
}

std::string PlutotvData::HttpPost(const std::string& url, const std::string& postData)
{
  return HttpRequest("POST", url, postData);
}

std::string PlutotvData::HttpRequest(const std::string& action,
                                     const std::string& url,
                                     const std::string& postData)
{
  Curl curl;
  int statusCode;

  curl.AddHeader("User-Agent", PLUTOTV_USER_AGENT);
  return HttpRequestToCurl(curl, action, url, postData, statusCode);
}

std::string PlutotvData::HttpRequestToCurl(Curl& curl,
                                           const std::string& action,
                                           const std::string& url,
                                           const std::string& postData,
                                           int& statusCode)
{
  kodi::Log(ADDON_LOG_DEBUG, "Http-Request: %s %s.", action.c_str(), url.c_str());
  std::string content;
  if (action == "POST")
  {
    content = curl.Post(url, postData, statusCode);
  }
  else if (action == "DELETE")
  {
    content = curl.Delete(url, postData, statusCode);
  }
  else
  {
    content = curl.Get(url, statusCode);
  }
  return content;
}
// END CURL helpers from zattoo addon


ADDON_STATUS PlutotvData::Create()
{
  kodi::Log(ADDON_LOG_DEBUG, "%s - Creating the pluto.tv PVR add-on", __FUNCTION__);

  LoadChannelData();
  m_curStatus = ADDON_STATUS_OK;
  return m_curStatus;
}

ADDON_STATUS PlutotvData::GetStatus()
{
  kodi::Log(ADDON_LOG_DEBUG, "pluto.tv function call: [%s]", __FUNCTION__);
  return m_curStatus;
}

ADDON_STATUS PlutotvData::SetSetting(const std::string& settingName,
                                     const kodi::CSettingValue& settingValue)
{
  return ADDON_STATUS_OK;
}

PVR_ERROR PlutotvData::GetCapabilities(kodi::addon::PVRCapabilities& capabilities)
{
  capabilities.SetSupportsEPG(true);
  capabilities.SetSupportsTV(true);

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PlutotvData::GetBackendName(std::string& name)
{
  name = "pluto.tv PVR add-on";
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PlutotvData::GetBackendVersion(std::string& version)
{
  version = STR(IPTV_VERSION);
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PlutotvData::GetConnectionString(std::string& connection)
{
  connection = "connected";
  return PVR_ERROR_NO_ERROR;
}

void PlutotvData::SetStreamProperties(std::vector<kodi::addon::PVRStreamProperty>& properties,
                                      const std::string& url,
                                      bool realtime)
{
  kodi::Log(ADDON_LOG_DEBUG, "[PLAY STREAM] url: %s", url.c_str());

  properties.emplace_back(PVR_STREAM_PROPERTY_STREAMURL, url);
  properties.emplace_back(PVR_STREAM_PROPERTY_INPUTSTREAM, "inputstream.adaptive");
  properties.emplace_back(PVR_STREAM_PROPERTY_ISREALTIMESTREAM, realtime ? "true" : "false");
  // HLS
  kodi::Log(ADDON_LOG_DEBUG, "[PLAY STREAM] hls");
  properties.emplace_back("inputstream.adaptive.manifest_type", "hls");
  properties.emplace_back(PVR_STREAM_PROPERTY_MIMETYPE, "application/x-mpegURL");
  properties.emplace_back("inputstream.adaptive.manifest_update_parameter", "full");
}

bool PlutotvData::LoadChannelData(void)
{
  kodi::Log(ADDON_LOG_DEBUG, "[load data] Login valid -> GET CHANNELS");

  std::string jsonChannels = HttpGet("https://api.pluto.tv/v2/channels.json");
  if (jsonChannels.size() == 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "[channels] ERROR - empty response");
    return PVR_ERROR_SERVER_ERROR;
  }
  jsonChannels = "{\"result\": " + jsonChannels + "}";
  kodi::Log(ADDON_LOG_DEBUG, "[channels] length: %i;", jsonChannels.length());
  kodi::Log(ADDON_LOG_DEBUG, "[channels] %s;", jsonChannels.c_str());
  kodi::Log(ADDON_LOG_DEBUG, "[channels] %s;",
            jsonChannels.substr(jsonChannels.size() - 40).c_str());

  // parse channels
  kodi::Log(ADDON_LOG_DEBUG, "[channels] parse channels");
  rapidjson::Document channelsDoc;
  channelsDoc.Parse(jsonChannels.c_str());
  if (channelsDoc.GetParseError())
  {
    kodi::Log(ADDON_LOG_ERROR, "[LoadChannelData] ERROR: error while parsing json");
    return PVR_ERROR_SERVER_ERROR;
  }
  kodi::Log(ADDON_LOG_DEBUG, "[channels] iterate channels");
  kodi::Log(ADDON_LOG_DEBUG, "[channels] size: %i;", channelsDoc["result"].Size());

  int i = 0;
  for (const auto& channel : channelsDoc["result"].GetArray())
  {
    /**
      {
      "_id":"5ad9b648e738977e2c312131",
      "slug":"aa02",
      "name":"Pluto TV Kids",
      "hash":"#KiddiDE",
      "number":251,
      "summary":"Lustige Cartoons, Kinderfilme \u0026 Klassiker sowie Filme für die ganze Familie sorgen für jede Menge Spaß und altersgerechte Unterhaltung. Ob für kleine Kids oder ältere Teens, bei Kids ist für jedes Kind und jede Familie garantiert das Richtige dabei.",
      "visibility":"everyone",
      "onDemandDescription":"",
      "category":"Kids",
      "plutoOfficeOnly":false,
      "directOnly":true,
      "chatRoomId":-1,
      "onDemand":false,
      "cohortMask":1023,
      "featuredImage":{ "path":"https://images.pluto.tv/channels/5ad9b648e738977e2c312131/featuredImage.jpg?w=1600\u0026h=900\u0026fm=jpg\u0026q=75\u0026fit=fill\u0026fill=blur"},
      "thumbnail":{"path":"https://images.pluto.tv/channels/5ad9b648e738977e2c312131/thumbnail.jpg?w=660\u0026h=660\u0026fm=jpg\u0026q=75\u0026fit=fill\u0026fill=blur" },
      "tile":{"path":"https://images.pluto.tv/channels/5ad9b648e738977e2c312131/tile.jpg"},
      "logo":{"path":"https://images.pluto.tv/channels/5ad9b648e738977e2c312131/logo.png?w=280\u0026h=80\u0026fm=png\u0026fit=fill"},
      "colorLogoSVG":{ "path":"https://images.pluto.tv/channels/5ad9b648e738977e2c312131/colorLogoSVG.svg" },
      "colorLogoPNG":{"path":"https://images.pluto.tv/channels/5ad9b648e738977e2c312131/colorLogoPNG.png"},
      "solidLogoSVG":{"path":"https://images.pluto.tv/channels/5ad9b648e738977e2c312131/solidLogoSVG.svg" },
      "solidLogoPNG":{"path":"https://images.pluto.tv/channels/5ad9b648e738977e2c312131/solidLogoPNG.png"},
      "featured":false,
      "featuredOrder":-1,
      "favorite":false,
      "isStitched":true,
      "stitched":{
         "urls":[{
               "type":"hls",
               "url":"https://service-stitcher.clusters.pluto.tv/stitch/hls/channel/5ad9b648e738977e2c312131/master.m3u8?advertisingId=\u0026appName=\u0026appVersion=unknown\u0026architecture=\u0026buildVersion=\u0026clientTime=\u0026deviceDNT=0\u0026deviceId=unknown\u0026deviceLat=49.9874\u0026deviceLon=8.4232\u0026deviceMake=\u0026deviceModel=\u0026deviceType=\u0026deviceVersion=unknown\u0026includeExtendedEvents=false\u0026marketingRegion=DE\u0026sid=\u0026userId="
            }],
         "sessionURL":"https://service-stitcher.clusters.pluto.tv/session/.json"
      }}, */

    std::string plutotvid = channel["_id"].GetString();
    ++i;
    PlutotvChannel plutotv_channel;
    plutotv_channel.iChannelNumber = i; // position
    kodi::Log(ADDON_LOG_DEBUG, "[channel] channelnr(pos): %i;", plutotv_channel.iChannelNumber);

    plutotv_channel.plutotvID = plutotvid;
    kodi::Log(ADDON_LOG_DEBUG, "[channel] pluto.tv ID: %s;", plutotv_channel.plutotvID.c_str());

    int uniqueId = Utils::GetChannelId(plutotvid.c_str());
    plutotv_channel.iUniqueId = uniqueId;
    kodi::Log(ADDON_LOG_DEBUG, "[channel] id: %i;", uniqueId);

    std::string displayName = channel["name"].GetString();
    plutotv_channel.strChannelName = displayName;
    kodi::Log(ADDON_LOG_DEBUG, "[channel] name: %s;", plutotv_channel.strChannelName.c_str());

    std::string logo = "";
    if (channel.HasMember("logo"))
    {
      logo = channel["logo"]["path"].GetString();
    }
    else if (channel.HasMember("colorLogoPNG"))
    {
      logo = channel["colorLogoPNG"]["path"].GetString();
    }

    plutotv_channel.strIconPath = logo;
    kodi::Log(ADDON_LOG_DEBUG, "[channel] logo: %s;", plutotv_channel.strIconPath.c_str());

    // plutotv_channel.strStreamURL
    if (channel.HasMember("stitched") && channel["stitched"].HasMember("urls") &&
        channel["stitched"]["urls"].Size() > 0)
    {
      std::string streamURL = channel["stitched"]["urls"][0]["url"].GetString();
      plutotv_channel.strStreamURL = streamURL;
      kodi::Log(ADDON_LOG_DEBUG, "[channel] streamURL: %s;", streamURL.c_str());
    }

    m_channels.push_back(plutotv_channel);
  }

  return true;
}

PVR_ERROR PlutotvData::GetChannelsAmount(int& amount)
{
  kodi::Log(ADDON_LOG_DEBUG, "pluto.tv function call: [%s]", __FUNCTION__);

  amount = m_channels.size();
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PlutotvData::GetChannels(bool radio, kodi::addon::PVRChannelsResultSet& results)
{
  kodi::Log(ADDON_LOG_DEBUG, "pluto.tv function call: [%s]", __FUNCTION__);

  for (const auto& channel : m_channels)
  {
    if (!radio)
    {
      kodi::addon::PVRChannel kodiChannel;

      kodiChannel.SetUniqueId(channel.iUniqueId);
      kodiChannel.SetIsRadio(false);
      kodiChannel.SetChannelNumber(channel.iChannelNumber);
      kodiChannel.SetChannelName(channel.strChannelName);
      kodiChannel.SetIconPath(channel.strIconPath);
      kodiChannel.SetIsHidden(false);

      results.Add(kodiChannel);
    }
  }
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PlutotvData::GetChannelStreamProperties(
    const kodi::addon::PVRChannel& channel, std::vector<kodi::addon::PVRStreamProperty>& properties)
{
  std::string strUrl = GetChannelStreamUrl(channel.GetUniqueId());
  kodi::Log(ADDON_LOG_DEBUG, "Stream URL -> %s", strUrl.c_str());
  PVR_ERROR ret = PVR_ERROR_FAILED;
  if (!strUrl.empty())
  {
    SetStreamProperties(properties, strUrl, true);
    ret = PVR_ERROR_NO_ERROR;
  }
  return ret;
}

std::string PlutotvData::GetSettingsUUID(std::string setting)
{
  std::string uuid = kodi::GetSettingString(setting);
  if (uuid.empty())
  {
    uuid = Utils::get_uuid();
    kodi::Log(ADDON_LOG_DEBUG, "uuid (generated): %s", uuid.c_str());
    kodi::SetSettingString(setting, uuid);
  }
  return uuid;
}

std::string PlutotvData::GetChannelStreamUrl(int uniqueId)
{
  for (const auto& thisChannel : m_channels)
  {
    if (thisChannel.iUniqueId == (int)uniqueId)
    {
      kodi::Log(ADDON_LOG_DEBUG, "Get live url for channel %s", thisChannel.strChannelName.c_str());

      std::string streamURL = thisChannel.strStreamURL;
      kodi::Log(ADDON_LOG_DEBUG, "URL source: %s", streamURL.c_str());


      if (Utils::ends_with(streamURL, "?deviceType="))
      {
        // lazy approach by plugin.video.plutotv
        streamURL = Utils::ReplaceAll(
            streamURL, "deviceType=",
            "deviceType=&deviceMake=&deviceModel=&&deviceVersion=unknown&appVersion=unknown&"
            "deviceDNT=0&userId=&advertisingId=&app_name=&appName=&buildVersion=&appStoreUrl=&"
            "architecture=&includeExtendedEvents=false");
      }

      //if 'sid' not in streamURL
      //streamURL = Utils::ReplaceAll(streamURL,"deviceModel=&","deviceModel=&sid="+PLUTOTV_SID+"&deviceId="+PLUTOTV_DEVICEID+"&");
      streamURL = Utils::ReplaceAll(streamURL, "deviceId=&",
                                    "deviceId=" + GetSettingsUUID("internal_deviceid") + "&");
      streamURL =
          Utils::ReplaceAll(streamURL, "sid=&", "sid=" + GetSettingsUUID("internal_sid") + "&");

      // generic
      streamURL = Utils::ReplaceAll(streamURL, "deviceType=&", "deviceType=web&");
      streamURL = Utils::ReplaceAll(streamURL, "deviceMake=&", "deviceMake=Chrome&");
      streamURL = Utils::ReplaceAll(streamURL, "deviceModel=&", "deviceModel=Chrome&");
      streamURL = Utils::ReplaceAll(streamURL, "appName=&", "appName=web&");

      return streamURL;
    }
  }
  return "";
}

PVR_ERROR PlutotvData::GetChannelGroupsAmount(int& amount)
{
  amount = static_cast<int>(0);
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PlutotvData::GetChannelGroups(bool radio, kodi::addon::PVRChannelGroupsResultSet& results)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR PlutotvData::GetChannelGroupMembers(const kodi::addon::PVRChannelGroup& group,
                                              kodi::addon::PVRChannelGroupMembersResultSet& results)
{
  return PVR_ERROR_NOT_IMPLEMENTED;
}

PVR_ERROR PlutotvData::GetEPGForChannel(int channelUid,
                                        time_t start,
                                        time_t end,
                                        kodi::addon::PVREPGTagsResultSet& results)
{
  // Find channel data
  for (unsigned int iChannelPtr = 0; iChannelPtr < m_channels.size(); iChannelPtr++)
  {
    PlutotvChannel& myChannel = m_channels.at(iChannelPtr);
    if (myChannel.iUniqueId != channelUid)
      continue;

    // Channel data found
    rapidjson::Document epgDoc;
    if (start == m_epg_cache_start && end == m_epg_cache_end)
    {
      epgDoc.CopyFrom(m_epg_cache_document, epgDoc.GetAllocator());
    }
    else
    {
      const time_t orig_start = start;
      const time_t now = std::time(nullptr);
      if (orig_start < now)
      {
        kodi::Log(ADDON_LOG_DEBUG, "[epg] adjusting start time to 'now' minus 3 hrs");
        start = now - 7200; // Pluto.tv API returns nothing if we step back (to wide) in time.
      }

      const std::tm* pstm = std::localtime(&start);
      // 2020-05-27T15:04:05Z
      char startTime[21] = "";
      std::strftime(startTime, 20, "%Y-%m-%dT%H:%M:%SZ", pstm);

      const std::tm* petm = std::localtime(&end);
      // 2020-05-27T15:04:05Z
      char endTime[21] = "";
      std::strftime(endTime, 20, "%Y-%m-%dT%H:%M:%SZ", petm);

      const std::string url = "http://api.pluto.tv/v2/channels?start=" + std::string(startTime) +
                              "&stop=" + std::string(endTime);

      std::string jsonEpg = HttpGet(url);
      kodi::Log(ADDON_LOG_DEBUG, "[epg-all] %s", jsonEpg.c_str());
      if (jsonEpg.size() == 0)
      {
        kodi::Log(ADDON_LOG_ERROR, "[epg] empty server response");
        return PVR_ERROR_SERVER_ERROR;
      }
      jsonEpg = "{\"result\": " + jsonEpg + "}";

      epgDoc.Parse(jsonEpg.c_str());
      if (epgDoc.GetParseError())
      {
        kodi::Log(ADDON_LOG_ERROR, "[GetEPG] ERROR: error while parsing json");
        return PVR_ERROR_SERVER_ERROR;
      }

      m_epg_cache_document.CopyFrom(epgDoc, m_epg_cache_document.GetAllocator());
      m_epg_cache_start = orig_start;
      m_epg_cache_end = end;
    }

    kodi::Log(ADDON_LOG_DEBUG, "[epg] iterate entries");

    kodi::Log(ADDON_LOG_DEBUG, "[epg] size: %i;", epgDoc["result"].Size());

    // Find EPG data
    for (const auto& epgChannel : epgDoc["result"].GetArray())
    {
      if (epgChannel["_id"].GetString() != myChannel.plutotvID)
        continue;

      // EPG data found
      for (const auto& epgData : epgChannel["timelines"].GetArray())
      {
        kodi::addon::PVREPGTag tag;

        //    "timelines":[{
        //          "_id":"5eccebf293483f0007d9ae18",
        //          "start":"2020-05-27T15:41:00.000Z",
        //          "stop":"2020-05-27T16:06:00.000Z",
        //          "title":"Planet Max: Die Affengrippe",
        //          "episode":{
        //             "_id":"5d0b449900557a40f64a71ee",
        //             "number":124,
        //             "description":"Nesmith hat einen Schnupfen. Max, der glaubt, dass Nesmith Luft verliert und bald platt sein wird, glaubt, dass nur eine Banane Nesmith retten kann. Und so machen sich Max, Aseefa und Doppy auf die Suche nach dem rettenden Heilmittel.",
        //             "duration":1500000,
        //             "genre":"News and Information",
        //             "subGenre":"Entertaining",
        //             "distributeAs":{ "AVOD":true },
        //             "clip":{  "originalReleaseDate":"2020-05-27T17:53:04.127Z"},
        //             "rating":"FSK-6",
        //             "name":"Die Affengrippe",
        //             "poster":{ "path":"http://images.pluto.tv/assets/images/default/vod.poster-default.jpg?w=694\u0026h=1000\u0026fm=jpg\u0026q=75\u0026fit=fill\u0026fill=blur" },
        //             "thumbnail":{ "path":"http://s3.amazonaws.com/silo.pluto.tv/origin/bluevo/nickelodeon/production/201906/20/nickelodeon_5d0a5767621cc_Planet-Max-DE-Die-Affengrippe-S1E124_1561019544860.jpg?w=440\u0026h=440\u0026fm=jpg\u0026q=75\u0026fit=fill\u0026fill=blur" },
        //             "liveBroadcast":false,
        //             "featuredImage":{ "path":"http://s3.amazonaws.com/silo.pluto.tv/origin/bluevo/nickelodeon/production/201906/20/nickelodeon_5d0a5767621cc_Planet-Max-DE-Die-Affengrippe-S1E124_1561019544860.jpg?w=1600\u0026h=900\u0026fm=jpg\u0026q=75\u0026fit=fill\u0026fill=blur" },
        //             "series":{
        //                "_id":"5d0b449100557a40f64a71ad",
        //                "name":"Planet Max",
        //                "type":"tv",
        //                "tile":{"path":"http://images.pluto.tv/series/5d0b449100557a40f64a71ad/tile.jpg?w=660\u0026h=660\u0026fm=jpg\u0026q=75\u0026fit=fill\u0026fill=blur" },
        //                "description":"Max, der beste Freund von Jimmy Neutron, schaut sich in Jimmys Labor um u.... Zeenu.",
        //                "summary":"Max, der beste Freund von Jimmy Neut ... chließlich auf dem Planeten Zeenu.",
        //                "featuredImage":{
        //                   "path":"http://images.pluto.tv/series/5d0b449100557a40f64a71ad/featuredImage.jpg?w=1600\u0026h=900\u0026fm=jpg\u0026q=75\u0026fit=fill\u0026fill=blur"
        //                } }  }   },

        // generate a unique boadcast id
        std::string epg_bid = epgData["_id"].GetString();
        kodi::Log(ADDON_LOG_DEBUG, "[epg] epg_bid: %s;", epg_bid.c_str());
        int dirtyID = Utils::GetIDDirty(epg_bid);
        kodi::Log(ADDON_LOG_DEBUG, "[epg] epg_bid dirty: %i;", dirtyID);
        tag.SetUniqueBroadcastId(dirtyID);

        // channel ID
        tag.SetUniqueChannelId(myChannel.iUniqueId);

        // set title
        tag.SetTitle(epgData["title"].GetString());
        kodi::Log(ADDON_LOG_DEBUG, "[epg] title: %s;", epgData["title"].GetString());

        // set startTime
        std::string startTime = epgData["start"].GetString();
        tag.SetStartTime(Utils::StringToTime(startTime));

        // set endTime
        std::string endTime = epgData["stop"].GetString();
        tag.SetEndTime(Utils::StringToTime(endTime));

        if (epgData.HasMember("episode"))
        {

          // set description
          if (epgData["episode"].HasMember("description") &&
              epgData["episode"]["description"].IsString())
          {
            tag.SetPlot(epgData["episode"]["description"].GetString());
            kodi::Log(ADDON_LOG_DEBUG, "[epg] description: %s;",
                      epgData["episode"]["description"].GetString());
          }

          // genre
          if (epgData["episode"].HasMember("genre") && epgData["episode"]["genre"].IsString())
          {
            tag.SetGenreType(EPG_GENRE_USE_STRING);
            tag.SetGenreDescription(epgData["episode"]["genre"].GetString());
          }

          // thumbnail
          if (epgData["episode"].HasMember("thumbnail") &&
              epgData["episode"]["thumbnail"]["path"].IsString())
          {
            tag.SetIconPath(epgData["episode"]["thumbnail"]["path"].GetString());
          }
        }

        results.Add(tag);
      }
      return PVR_ERROR_NO_ERROR;
    }
    // EPG for channel not found. This is not an error. Channel might just have no EPG data.
    return PVR_ERROR_NO_ERROR;
  }

  kodi::Log(ADDON_LOG_ERROR, "[GetEPG] ERROR: channel not found");
  return PVR_ERROR_INVALID_PARAMETERS;
}

ADDONCREATOR(PlutotvData)
