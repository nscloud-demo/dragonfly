// Copyright 2021, Roman Gershman.  All rights reserved.
// See LICENSE for licensing terms.
//

#pragma once

#include "base/varz_value.h"
#include "server/command_registry.h"
#include "server/engine_shard_set.h"
#include "util/http/http_handler.h"
#include "server/memcache_parser.h"
#include "server/server_family.h"

namespace util {
class AcceptServer;
}  // namespace util

namespace dfly {

class Service {
 public:
  using error_code = std::error_code;

  struct InitOpts {
    bool disable_time_update;

    InitOpts() : disable_time_update{false} {
    }
  };

  explicit Service(util::ProactorPool* pp);
  ~Service();

  void RegisterHttp(util::HttpListenerBase* listener);

  void Init(util::AcceptServer* acceptor, const InitOpts& opts = InitOpts{});

  void Shutdown();

  void DispatchCommand(CmdArgList args, ConnectionContext* cntx);
  void DispatchMC(const MemcacheParser::Command& cmd, std::string_view value,
                  ConnectionContext* cntx);

  uint32_t shard_count() const {
    return shard_set_.size();
  }

  // Used by tests.
  bool IsLocked(DbIndex db_index, std::string_view key) const;

  EngineShardSet& shard_set() {
    return shard_set_;
  }

  util::ProactorPool& proactor_pool() {
    return pp_;
  }

 private:

  static void Quit(CmdArgList args, ConnectionContext* cntx);
  void Exec(CmdArgList args, ConnectionContext* cntx);
  static void Multi(CmdArgList args, ConnectionContext* cntx);

  void RegisterCommands();

  base::VarzValue::Map GetVarzStats();

  CommandRegistry registry_;
  EngineShardSet shard_set_;
  util::ProactorPool& pp_;
  ServerFamily server_family_;
};

}  // namespace dfly