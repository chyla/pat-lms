/*
 * Copyright 2016 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#pragma once

#include "detail/general_database_functions_interface.h"

#include <memory>

#include "detail/sqlite_wrapper_interface.h"
#include "src/database/database.h"

namespace database
{

class GeneralDatabaseFunctions;
typedef std::shared_ptr<GeneralDatabaseFunctions> GeneralDatabaseFunctionsPtr;

class GeneralDatabaseFunctions : public detail::GeneralDatabaseFunctionsInterface {
 public:
  virtual ~GeneralDatabaseFunctions() = default;

  static GeneralDatabaseFunctionsPtr Create(DatabasePtr database,
                                            detail::SQLiteWrapperInterfacePtr sqlite_wrapper);

  void CreateTables() override;

  void AddTime(const ::type::Time &t) override;
  ::database::type::RowId AddAndGetTimeId(const ::type::Time &time) override;
  ::database::type::RowId GetTimeId(const ::type::Time &t) override;
  const ::type::Time GetTimeById(::database::type::RowId id) override;

  void AddDate(const ::type::Date &date) override;
  ::database::type::RowId AddAndGetDateId(const ::type::Date &date) override;
  ::database::type::RowId GetDateId(const ::type::Date &date) override;
  ::database::type::RowIds GetDateRangeIds(const ::type::Date &from, const ::type::Date &to);
  ::type::Date GetDateById(const ::database::type::RowId &id) override;

  void AddAgentName(const std::string &name) override;
  type::AgentNames GetAgentNames() override;
  ::database::type::RowIds GetAgentsIds() override;
  ::database::type::RowId AddAndGetAgentNameId(const std::string &name) override;
  ::database::type::RowId GetAgentNameId(const std::string &name) override;
  std::string GetAgentNameById(const ::database::type::RowId &id) override;

 private:
  GeneralDatabaseFunctions(DatabasePtr database,
                           detail::SQLiteWrapperInterfacePtr sqlite_wrapper);

  DatabasePtr database_;
  detail::SQLiteWrapperInterfacePtr sqlite_wrapper_;
};

}
