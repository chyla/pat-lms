/*
 * Copyright 2016 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#ifndef UNCONFIGURED_AGENT_H
#define UNCONFIGURED_AGENT_H

#include "src/database/type/row_id.h"
#include "src/database/type/agent_name.h"

#include <vector>

namespace bash
{

namespace domain
{

namespace type
{

struct UnconfiguredAgent {
  ::database::type::RowId id;
  ::database::type::AgentName name;
};

typedef std::vector<UnconfiguredAgent> UnconfiguredAgents;

}

}

}

#endif /* UNCONFIGURED_AGENT_H */
