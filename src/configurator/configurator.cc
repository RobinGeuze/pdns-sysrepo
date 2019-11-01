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

#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>

#include <boost/filesystem.hpp>
#include <mstch/mstch.hpp>
#include <spdlog/spdlog.h>

#include "configurator.hh"
#include "util.hh"

using namespace std;
namespace fs = boost::filesystem;

namespace pdns_conf
{
static const string pdns_conf_template = R"(
local-address =
{{ #local-address }}
local-address += {{ address }}:{{ port }} # {{ name }}
{{ /local-address }}
master = {{ master }}
slave = {{ slave }}
)";

PdnsServerConfig::PdnsServerConfig(const libyang::S_Data_Node &node) {
  // spdlog::debug("root node name={} schema_type={} path={}", node->schema()->name(), libyangNodeType2String(node->schema()->nodetype()), node->schema()->path());

  libyang::S_Data_Node_Leaf_List leaf;
  string nodename(node->schema()->name());

  if (node->schema()->nodetype() == LYS_CONTAINER && nodename == "pdns-server") {
    auto child = node->child();
    do {
      nodename = child->schema()->name();
      // spdlog::debug("node name={} schema_type={} path={}", child->schema()->name(), libyangNodeType2String(child->schema()->nodetype()), child->schema()->path());
      if (child->schema()->nodetype() == LYS_LEAF) {
        leaf = make_shared<libyang::Data_Node_Leaf_List>(child);
      }
      if(nodename == "master") {
        master = leaf->value()->bln();
      }
      if (nodename == "slave") {
        slave = leaf->value()->bln();
      }
      if (nodename == "listen-addresses") {
        listenAddress la;
        for (const auto &n: child->tree_dfs()) {
          if(n->schema()->nodetype() == LYS_LEAF) {
            string leafName(n->schema()->name());
            leaf = make_shared<libyang::Data_Node_Leaf_List>(n);
            if(leafName == "name") {
              la.name = leaf->value()->string();
            }
            if(leafName == "ip-address") {
              la.address = leaf->value()->string();
            }
            if(leafName == "port") {
              la.port = leaf->value()->uint16();
            }
          }
        }
        listenAddresses.push_back(la);
      }
    } while(child = child->next());
  }
}

void PdnsServerConfig::writeToFile(const string &fpath) {
  spdlog::debug("Attempting to create configuration file={}", fpath);
  auto p = fs::path(fpath);
  auto d = p.remove_filename();
  if (!fs::is_directory(d)) {
    // TODO find better exception
    throw range_error(d.string() + " is not a directory");
  }

  mstch::array laddrs;
  for (const auto& la : listenAddresses) {
    laddrs.push_back(mstch::map{
      {"name", la.name},
      {"address", la.address},
      {"port", la.port}});
  }

  mstch::map ctx{
    {"master", bool2str(master)},
    {"slave", bool2str(slave)},
    {"local-address", laddrs}
  };

  spdlog::trace("Generated config:\n{}", mstch::render(pdns_conf_template, ctx));
  // TODO tmpfile
  std::ofstream outputFile(fpath);
  if (!outputFile) {
    throw runtime_error("Unable to open output file '" + fpath + "'");
  }

  outputFile << mstch::render(pdns_conf_template, ctx);
  outputFile.close();
  spdlog::trace("Written config file {}", fpath);
}

string PdnsServerConfig::bool2str(const bool b) {
  return b ? "yes" : "no";
}
} // namespace pdns_conf