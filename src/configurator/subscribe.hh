/*
 * Copyright 2019 Pieter Lexis <pieter.lexis@powerdns.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <sysrepo-cpp/Session.hpp>
#include <spdlog/spdlog.h>
#include <sdbusplus/message.hpp>

#include "configurator.hh"
#include "config.h"
#include "sr_wrapper/session.hh"

#include "ApiClient.h"
#include "model/Zone.h"

using std::map;
using std::shared_ptr;
using std::string;
using std::vector;
using std::make_shared;
namespace pdns_api = org::openapitools::client::api;
namespace pdns_api_model = org::openapitools::client::model;

namespace pdns_conf
{
class ZoneCB : public sysrepo::Callback
{
  public:
  ZoneCB(shared_ptr<pdns_api::ApiClient> &apiClient) :
    sysrepo::Callback(),
    d_apiClient(apiClient)
    {};
  ~ZoneCB(){};

  /**
   * @brief Callback called when an application is requesting operational or config data
   * 
   * @see sysrepo::Callback::oper_get_items
   * 
   * @param session 
   * @param module_name 
   * @param path 
   * @param request_xpath 
   * @param request_id 
   * @param parent 
   * @param private_data 
   * @return int 
   */
  int oper_get_items(sysrepo::S_Session session, const char* module_name,
    const char* path, const char* request_xpath,
    uint32_t request_id, libyang::S_Data_Node& parent, void* private_data) override;

  private:
  shared_ptr<pdns_api::ApiClient> d_apiClient;
};

class ServerConfigCB : public sysrepo::Callback
{
public:
/**
 * @brief Construct a new ServerConfigCB object
 * 
 * This class implements the sysrepo::Callback specifically for the
 * PowerDNS Authoritative Server. An instance of this should be used in
 * a sysrepo::Subscribe, it then receives the changes in the datastore.
 * Please use pdns_conf::getServerConfigCB to create an instance.
 * 
 * @param privData  Data that the functions can use
 */
  ServerConfigCB(const map<string, string>& privData, shared_ptr<pdns_api::ApiClient> &apiClient) :
    sysrepo::Callback(),
    d_apiClient(apiClient),
    privData(privData)
    {};
  ~ServerConfigCB(){};


  /**
   * @brief Callback for when data in a module changes.
   * 
   * @see sysrepo::Callback::module_change
   * 
   * @param session 
   * @param module_name 
   * @param xpath 
   * @param event 
   * @param request_id 
   * @param private_data 
   * @return int 
   */
  int module_change(sysrepo::S_Session session, const char* module_name,
    const char* xpath, sr_event_t event,
    uint32_t request_id, void* private_data) override;

private:
  /**
   * @brief Return a file path to a tempfile
   * 
   * @param request_id  A number that should be unique for the
   *                    whole transaction
   * @return string 
   */
  string tmpFile(const uint32_t request_id);

  /**
   * @brief Restarts a systemd service
   * 
   * This function uses the dbus interface to systemd to call the
   * RestartUnit method. It will then wait to see if the restart succeeded.
   * 
   * @param service  The service to restart
   * @throw std::runtime_error  When communication over dbus fails
   */
  void restartService(const string& service);

  /**
   * @brief API client
   */
  shared_ptr<pdns_api::ApiClient> d_apiClient;

  /**
   * @brief Private data that this class can use
   */
  map<string, string> privData;

  /**
   * @brief Handles the additions and removals of zones.
   * 
   * This will add the zones that are removed or added to the zonesCreated and zonesRemoved
   * lists
   * 
   * @param sess    The session passed to the callback, used to get data if required
   * 
   * @throw 
   */
  void changeZoneAddAndDelete(sysrepo::S_Session &sess);

  /**
   * @brief Uses the API to add and remove zones
   * 
   */
  void doneZoneAddAndDelete();

  vector<pdns_api_model::Zone> zonesCreated;
  vector<string> zonesRemoved;
};

/**
 * @return sysrepo::S_Callback 
 */

/**
 * @brief Get a shared pointer to a ServerConfigCB object
 * 
 * @param fpath                 The path to the PowerDNS Authoritative Server
 *                              configuration file
 * @param serviceName           Name of the PowerDNS service
 * @return std::shared_ptr<ServerConfigCB> 
 */
std::shared_ptr<ServerConfigCB> getServerConfigCB(const string& fpath, const string& serviceName, shared_ptr<pdns_api::ApiClient> &apiClient);

/**
 * @brief Get a new ZoneCB object wrapped in a shared_ptr
 * 
 * @param url     Domain name for the API, "http://" wil be prepended, "/api/v1" will be appended
 * @param passwd  The API Key to use
 * @return std::shared_ptr<ZoneCB> 
 */
std::shared_ptr<ZoneCB> getZoneCB(shared_ptr<pdns_api::ApiClient> &apiClient);

} // namespace pdns_conf