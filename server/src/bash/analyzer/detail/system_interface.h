/*
 * Copyright 2016 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#ifndef BASH_ANALYZER_DETAIL_SYSTEM_INTERFACE_H
#define BASH_ANALYZER_DETAIL_SYSTEM_INTERFACE_H

#include <memory>
#include <ctime>

namespace bash
{

namespace analyzer
{

namespace detail
{

class SystemInterface {
 public:
  virtual ~SystemInterface() = default;

  virtual time_t Time(time_t *t) = 0;

  virtual struct tm* LocalTime(const time_t *timep) = 0;
};

typedef std::shared_ptr<SystemInterface> SystemInterfacePtr;

}

}

}

#endif /* BASH_ANALYZER_DETAIL_SYSTEM_INTERFACE_H */
