/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <vector>

#include "sc-memory/sc_memory.hpp"

using CommandParameters = std::map<std::string, std::vector<std::string>>;

class ScComponentManagerCommand
{
public:
  using ScAddrUnorderedSet = std::unordered_set<ScAddr, ScAddrHashFunc<sc_uint32>>;
  virtual ScAddrUnorderedSet Execute(ScMemoryContext * context, ScAddr const & actionAddr) = 0;

  virtual ~ScComponentManagerCommand() = default;
};
