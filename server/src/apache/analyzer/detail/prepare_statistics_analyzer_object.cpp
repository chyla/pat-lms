/*
 * Copyright 2016 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "prepare_statistics_analyzer_object.h"

#include <boost/log/trivial.hpp>
#include <patlms/util/distance.h>

#include "system.h"
#include "session_length.h"

namespace apache
{

namespace analyzer
{

namespace detail
{

PrepareStatisticsAnalyzerObjectPtr PrepareStatisticsAnalyzerObject::Create(::apache::database::detail::DatabaseFunctionsInterfacePtr database_functions) {
  BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::Create: Function call";
  auto system = System::Create();

  return PrepareStatisticsAnalyzerObjectPtr(new PrepareStatisticsAnalyzerObject(database_functions, system));
}

PrepareStatisticsAnalyzerObjectPtr PrepareStatisticsAnalyzerObject::Create(::apache::database::detail::DatabaseFunctionsInterfacePtr database_functions,
                                                                           SystemInterfacePtr system_interface) {
  BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::Create: Function call";

  return PrepareStatisticsAnalyzerObjectPtr(new PrepareStatisticsAnalyzerObject(database_functions, system_interface));
}

void PrepareStatisticsAnalyzerObject::Prepare() {
  BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::Prepare: Function call";

  for (auto agent_name : database_functions_->GetAgentNames()) {
    BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::Prepare: Preparing agent: " << agent_name;

    for (auto virtualhost_name : database_functions_->GetVirtualhostNames(agent_name)) {
      BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::Prepare: Preparing virtualhost: " << virtualhost_name;

      CreateStatistics(agent_name, virtualhost_name);
    }
  }
}

PrepareStatisticsAnalyzerObject::PrepareStatisticsAnalyzerObject(::apache::database::detail::DatabaseFunctionsInterfacePtr database_functions,
                                                                 SystemInterfacePtr system_interface) :
database_functions_(database_functions),
system_interface_(system_interface) {
}

void PrepareStatisticsAnalyzerObject::CreateStatistics(const ::database::type::AgentName &agent_name,
                                                       const ::database::type::VirtualhostName &virtualhost_name) {
  BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::CreateStatistics: Function call";
  using ::database::type::RowsCount;
  constexpr RowsCount max_rows_in_memory = 100;

  ::type::Date date_from;
  auto is_stats = database_functions_->IsLastRunSet(::apache::type::LastRunType::STATISTICS_CALCULATION);
  if (is_stats) {
    date_from = database_functions_->GetLastRun(::apache::type::LastRunType::STATISTICS_CALCULATION);
    BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::CreateStatistics: Found last statistics calculation date " << date_from;
  }
  else {
    date_from.Set(1, 1, 2016);
    BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::CreateStatistics: Last statistics calculation date not found - assuming " << date_from;
  }

  auto date_to = GetCurrentDate();
  BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::CreateStatistics: Current date " << date_to;

  for (auto t = date_from.GetTomorrowDate(); t < date_to || t == date_to; t = t.GetTomorrowDate()) {
    BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::CreateStatistics: Analyzing day " << t;

    if (database_functions_->AreStatisticsCreatedFor(t) == false) {
      BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::CreateStatistics: Statistics not created for " << t;

      RowsCount summary_logs_count = database_functions_->GetLogsCount(agent_name, virtualhost_name,
                                                                       t, t);

      BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::CreateStatistics: Found " << summary_logs_count << " logs";

      for (RowsCount i = 0; i < summary_logs_count / max_rows_in_memory; ++i) {
        CalculateStatistics(agent_name, virtualhost_name, t, t, max_rows_in_memory, i);
      }

      auto offset = summary_logs_count - (summary_logs_count % max_rows_in_memory);
      auto left_logs_count = summary_logs_count % max_rows_in_memory;

      CalculateStatistics(agent_name,
                          virtualhost_name,
                          t,
                          t,
                          left_logs_count,
                          offset);

      SaveAllSessions();

      database_functions_->MarkStatisticsAsCreatedFor(t);
    }
    else {
      BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::CreateStatistics: Statistics already created for " << t << ", nothing to do";
    }
  }
}

void PrepareStatisticsAnalyzerObject::CalculateStatistics(const ::database::type::AgentName &agent_name,
                                                          const ::database::type::VirtualhostName &virtualhost_name,
                                                          const ::type::Date &from,
                                                          const ::type::Date &to,
                                                          ::database::type::RowsCount count,
                                                          ::database::type::RowId offset) {
  BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::CalculateStatistics: Function call";
  BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::CalculateStatistics: Calculating statistics for " << count << " logs with offset " << offset << " from " << from.ToString() << " to " << to.ToString();

  auto logs = database_functions_->GetLogs(agent_name, virtualhost_name,
                                           from, to,
                                           count, offset);

  UniqueSessionId usi;
  for (auto log_entry : logs) {
    usi.first = log_entry.client_ip;
    usi.second = log_entry.agent_name;

    BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::CalculateStatistics: Found new log with id: " << log_entry.id;
    BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::CalculateStatistics: Log client ip and agent name: " << log_entry.client_ip << " ; " << log_entry.agent_name;

    if (sessions_statistics_.find(usi) != sessions_statistics_.end()) {
      BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::CalculateStatistics: Session statistics found in memory";

      ::apache::type::ApacheSessionEntry &session = sessions_statistics_.at(usi);

      if (IsInThisSameSession(session, log_entry)) {
        session.bandwidth_usage += log_entry.bytes;
        session.errors_count += static_cast<int> (IsErrorCode(log_entry.status_code));
        session.requests_count += 1;
        session.error_percentage = session.errors_count * 100. / session.requests_count;
        session.session_length = ::util::Distance(session.session_start.GetTime(), log_entry.time.GetTime());
      }
      else {
        BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::CalculateStatistics: Session statistics found in memory, but removing";
        calculated_sessions_statistics_.push_back(session);
        sessions_statistics_.erase(usi);

        ::apache::type::ApacheSessionEntry session;
        session.agent_name = log_entry.agent_name;
        session.bandwidth_usage = log_entry.bytes;
        session.client_ip = log_entry.client_ip;
        session.errors_count = static_cast<int> (IsErrorCode(log_entry.status_code));
        session.error_percentage = session.errors_count * 100;
        session.is_anomaly = false;
        session.requests_count = 1;
        session.session_length = 0;
        session.session_start = log_entry.time;
        session.useragent = log_entry.user_agent;
        session.virtualhost = log_entry.virtualhost;

        sessions_statistics_[usi] = session;
      }
    }
    else {
      BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::CalculateStatistics: Session statistics not found in memory";

      ::apache::type::ApacheSessionEntry session;
      session.agent_name = log_entry.agent_name;
      session.bandwidth_usage = log_entry.bytes;
      session.client_ip = log_entry.client_ip;
      session.errors_count = static_cast<int> (IsErrorCode(log_entry.status_code));
      session.error_percentage = session.errors_count * 100;
      session.is_anomaly = false;
      session.requests_count = 1;
      session.session_length = 0;
      session.session_start = log_entry.time;
      session.useragent = log_entry.user_agent;
      session.virtualhost = log_entry.virtualhost;

      sessions_statistics_[usi] = session;
    }
  }

  BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::CalculateStatistics: Done";
}

::type::Date PrepareStatisticsAnalyzerObject::GetCurrentDate() const {
  time_t t = system_interface_->Time(nullptr);
  struct tm *now = system_interface_->LocalTime(&t);

  return ::type::Date::Create(now->tm_mday, now->tm_mon + 1, now->tm_year + 1900);
}

bool PrepareStatisticsAnalyzerObject::IsErrorCode(const int &status_code) const {
  return (status_code >= 400) && (status_code <= 511);
}

bool PrepareStatisticsAnalyzerObject::IsInThisSameSession(const ::apache::type::ApacheSessionEntry &session,
                                                          const ::type::ApacheLogEntry &log_entry) {
  return (::util::Distance(session.session_start.GetTime(), log_entry.time.GetTime()) < SESSION_LENGTH) &&
      (session.session_start.GetDate() == log_entry.time.GetDate());
}

void PrepareStatisticsAnalyzerObject::SaveAllSessions() {
  BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::PrepareStatisticsAnalyzerObject::SaveAllSessions: Function call";

  for (auto element : sessions_statistics_) {
    calculated_sessions_statistics_.push_back(element.second);
  }

  database_functions_->AddSessionStatistics(calculated_sessions_statistics_);

  sessions_statistics_.clear();
  calculated_sessions_statistics_.clear();
}

}

}

}
