/*
 * Copyright 2016 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#pragma once

#include <slas/type/bash_log_entry.h>

#include "src/database/type/row_id.h"
#include "src/bash/database/type/uid.h"
#include "src/bash/database/type/command_name.h"
#include "src/bash/database/type/anomaly_detection_configuration.h"
#include "src/bash/database/detail/entity/daily_system_statistic.h"
#include "src/bash/database/detail/entity/daily_user_statistic.h"
#include "src/bash/database/detail/entity/daily_user_command_statistic.h"
#include "src/bash/database/detail/entity/command_statistic.h"
#include "src/bash/database/detail/entity/system_user.h"
#include "src/database/entity/agent_name.h"
#include "src/bash/database/detail/type/daily_user_named_command_statistic.h"

#include <memory>

namespace bash
{

namespace database
{

namespace detail
{

class DatabaseFunctionsInterface {
 public:
  virtual ~DatabaseFunctionsInterface() = default;

  virtual void CreateTables() = 0;

  virtual void AddSystemUser(::bash::database::type::UID uid) = 0;
  virtual ::database::type::RowId GetSystemUserId(::bash::database::type::UID uid) = 0;
  virtual ::bash::database::detail::entity::SystemUser GetSystemUserById(::database::type::RowId id) = 0;
  virtual ::database::type::RowIds GetSystemUsersIdsFromLogs(::database::type::RowId agent_name_id) = 0;

  virtual void AddCommand(const ::bash::database::type::CommandName &command) = 0;
  virtual ::database::type::RowId GetCommandId(const ::bash::database::type::CommandName &command) = 0;
  virtual ::database::type::RowIds GetAllCommandsIds() = 0;
  virtual ::bash::database::type::CommandName GetCommandNameById(::database::type::RowId id) = 0;

  virtual void AddLog(const ::type::BashLogEntry &log_entry) = 0;
  virtual ::database::type::RowsCount CountCommandsForDailySystemStatistic(::database::type::RowId agent_name_id,
                                                                           ::database::type::RowId date_id,
                                                                           ::database::type::RowId command_id) = 0;
  virtual ::database::type::RowsCount CountCommandsForUserDailyStatisticFromLogs(::database::type::RowId agent_name_id,
                                                                                 ::database::type::RowId date_id,
                                                                                 ::database::type::RowId user_id,
                                                                                 ::database::type::RowId command_id) = 0;

  virtual void AddDailySystemStatistic(const entity::DailySystemStatistic &statistics) = 0;
  virtual void AddDailySystemStatistics(const entity::DailySystemStatistics &statistics) = 0;
  virtual ::database::type::RowIds GetDateIdsWithoutCreatedDailySystemStatistic(::database::type::RowId agent_name_id) = 0;

  virtual ::database::type::RowIds GetAgentIdsWithoutConfiguration() = 0;
  virtual ::database::type::RowIds GetAgentsIdsWithConfiguration() = 0;
  virtual void AddDailyUserStatistic(const ::bash::database::detail::entity::DailyUserStatistic &us) = 0;
  virtual ::database::type::RowId GetDailyUserStatisticId(::database::type::RowId agent_name_id,
                                                          ::database::type::RowId user_id,
                                                          ::database::type::RowId date_id) = 0;
  virtual entity::DailyUserStatistic GetDailyUserStatisticById(const ::database::type::RowId &id) = 0;
  virtual void SetDailyUserStatisticsClassification(const ::database::type::RowIds &ids, ::database::type::Classification classification) = 0;
  virtual void AddDailyUserCommandStatistic(const ::bash::database::detail::entity::DailyUserCommandStatistic &ucs) = 0;
  virtual ::database::entity::AgentNames GetAgentsWithExistingDailyUserStatistics() = 0;
  virtual ::database::entity::AgentNames GetAgentsWithExistingDailyUserStatisticsNotInLearningSet() = 0;
  virtual ::database::type::RowsCount CountDailyUserStatisticsForAgentWithClassification(::database::type::RowId agent_name_id,
                                                                                         ::database::type::Classification classification) = 0;
  virtual ::bash::database::detail::entity::DailyUserStatistics GetDailyUserStatisticsForAgentWithClassification(::database::type::RowId agent_name_id,
                                                                                                                 ::database::type::Classification classification,
                                                                                                                 ::database::type::RowsCount limit,
                                                                                                                 ::database::type::RowsCount offset) = 0;

  virtual ::bash::database::type::AnomalyDetectionConfigurations GetAnomalyDetectionConfigurations() = 0;
  virtual void RemoveAnomalyDetectionConfiguration(::database::type::RowId id) = 0;
  virtual void AddAnomalyDetectionConfiguration(const ::bash::database::type::AnomalyDetectionConfiguration &configuration) = 0;
  virtual ::database::type::RowId GetConfigurationIdForAgent(::database::type::RowId agent_id) = 0;
  virtual void RemoveAllCommandsFromConfiguration(::database::type::RowId configuration_id) = 0;
  virtual void AddDefaultCommandsToConfiguration(::database::type::RowId configuration_id) = 0;
  virtual void MarkConfigurationAsUnchanged(::database::type::RowId configuration_id) = 0;
  virtual void MarkConfigurationAsChanged(::database::type::RowId configuration_id) = 0;

  virtual void AddCommandStatistic(const entity::CommandStatistic &statistic) = 0;
  virtual void AddCommandsStatistics(const entity::CommandsStatistics &statistics) = 0;
  virtual bool IsCommandStatisticExist(::database::type::RowId agent_name_id,
                                       ::database::type::RowId command_id,
                                       ::database::type::RowId begin_date_id,
                                       ::database::type::RowId end_date_id) = 0;
  virtual entity::CommandsStatistics GetCommandsStatistics(::database::type::RowId agent_name_id,
                                                           ::database::type::RowId begin_date_id,
                                                           ::database::type::RowId end_date_id) = 0;
  virtual entity::CommandsStatistics GetCommandsStatistics(::database::type::RowId configuration_id) = 0;
  virtual ::database::type::RowIds GetMarkedCommandsIds(::database::type::RowId configuration_id) = 0;
  virtual ::database::type::RowsCount CommandSummary(::database::type::RowId agent_name_id,
                                                     ::database::type::RowId command_id,
                                                     ::database::type::RowIds date_range_ids) = 0;
  virtual void AddSelectedCommandsIds(::database::type::RowId configuration_id,
                                      ::database::type::RowIds command_names_ids) = 0;

  virtual ::database::type::RowIds GetNotCalculatedDatesIdsFromLogs(::database::type::RowId agent_name_id,
                                                                    ::database::type::RowId user_id) = 0;
  virtual ::database::type::RowIds GetCommandsIdsFromLogs(::database::type::RowId agent_name_id,
                                                          ::database::type::RowId user_id,
                                                          ::database::type::RowId date_id) = 0;

  virtual void AddDailyUserStatisticsToConfiguration(::database::type::RowId configuration_id,
                                                     const ::database::type::RowIds &date_range_ids) = 0;
  virtual void RemoveDailyStatisticsFromConfiguration(::database::type::RowId configuration_id) = 0;
  virtual ::bash::database::detail::entity::DailyUserStatistics GetDailyUserStatisticsFromConfiguration(::database::type::RowId configuration_id) = 0;

  virtual ::bash::database::detail::entity::DailyUserStatistics GetDailyUserStatisticsForAgent(::database::type::RowId agent_name_id,
                                                                                               const ::database::type::RowIds &date_range_ids) = 0;
  virtual ::bash::database::detail::entity::DailyUserStatistics GetDailyUserStatisticsWithoutLearningSetForAgent(::database::type::RowId agent_name_id,
                                                                                                                 const ::database::type::RowIds &date_range_ids) = 0;

  virtual ::database::type::RowIds GetUsersIdsFromSelectedDailyStatisticsInConfiguration(::database::type::RowId configuration_id) = 0;
  virtual ::database::type::RowId CountSelectedDailyStatisticsWithoutUnknownClassificationInConfiguration(::database::type::RowId configuration_id) = 0;
  virtual ::database::type::RowsCount CountSelectedDailyUserStatisticsWithoutUnknownClassificationFromConfigurationByUser(::database::type::RowId configuration_id,
                                                                                                                          ::database::type::RowId user_id) = 0;
  virtual ::bash::database::detail::entity::DailyUserStatistics GetSelectedDailyUserStatisticsWithoutUnknownClassificationFromConfigurationByUser(::database::type::RowId configuration_id,
                                                                                                                                                  ::database::type::RowId user_id,
                                                                                                                                                  ::database::type::RowsCount limit,
                                                                                                                                                  ::database::type::RowsCount offset) = 0;
  virtual ::bash::database::detail::entity::DailyUserCommandsStatistics GetSelectedDailyUserCommandsStatistics(::database::type::RowId statistic_id) = 0;

  virtual ::bash::database::detail::type::DailyUserNamedCommandsStatistics GetDailyUserNamedCommandsStatistics(::database::type::RowId daily_user_statistic_id) = 0;
};

typedef std::shared_ptr<DatabaseFunctionsInterface> DatabaseFunctionsInterfacePtr;

}

}

}
