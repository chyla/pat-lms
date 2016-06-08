/*
 * Copyright 2016 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "apache_analyzer_object.h"

#include <boost/log/trivial.hpp>

#include <patlms/util/distance.h>
#include "detail/session_length.h"
#include "detail/prepare_statistics_analyzer_object.h"
#include "detail/knn_analyzer_object.h"

namespace apache
{

namespace analyzer
{

ApacheAnalyzerObjectPtr ApacheAnalyzerObject::Create(::database::detail::GeneralDatabaseFunctionsInterfacePtr general_database_functions,
                                                     ::apache::database::DatabaseFunctionsPtr database_functions) {
  BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::ApacheAnalyzerObject::Create: Function call";
  auto system_interface = detail::System::Create();

  return Create(general_database_functions, database_functions, system_interface);
}

ApacheAnalyzerObjectPtr ApacheAnalyzerObject::Create(::database::detail::GeneralDatabaseFunctionsInterfacePtr general_database_functions,
                                                     ::apache::database::DatabaseFunctionsPtr database_functions,
                                                     detail::SystemInterfacePtr system_interface) {
  BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::ApacheAnalyzerObject::Create: Function call";

  return ApacheAnalyzerObjectPtr(new ApacheAnalyzerObject(general_database_functions, database_functions, system_interface));
}

void ApacheAnalyzerObject::Analyze() {
  BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::ApacheAnalyzerObject::Analyze: Function call";

  auto statistics = detail::PrepareStatisticsAnalyzerObject::Create(database_functions_);
  auto knn_analyzer = detail::KnnAnalyzerObject::Create(general_database_functions_,
                                                        database_functions_);

  auto now = GetCurrentTimestamp();

  if (ShouldRun(now)) {
    BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::ApacheAnalyzerObject::Analyze: ShouldRun: true";
    statistics->Prepare(now);
    knn_analyzer->Analyze(now);

    database_functions_->SetLastRun(::apache::type::LastRunType::STATISTICS_CALCULATION, now);
  }
  else {
    BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::ApacheAnalyzerObject::Analyze: ShouldRun: false";
    BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::ApacheAnalyzerObject::Analyze: Not running";
  }
}

ApacheAnalyzerObject::ApacheAnalyzerObject(::database::detail::GeneralDatabaseFunctionsInterfacePtr general_database_functions,
                                           ::apache::database::DatabaseFunctionsPtr database_functions,
                                           detail::SystemInterfacePtr system_interface) :
general_database_functions_(general_database_functions),
database_functions_(database_functions),
system_interface_(system_interface) {
}

bool ApacheAnalyzerObject::ShouldRun(const ::type::Timestamp &now) {
  BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::ApacheAnalyzerObject::ShouldRun: Function call";

  if (database_functions_->IsLastRunSet(::apache::type::LastRunType::STATISTICS_CALCULATION) == false) {
    BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::ApacheAnalyzerObject::ShouldRun: Last run not set, returning true";
    return true;
  }

  auto last_run = database_functions_->GetLastRun(::apache::type::LastRunType::STATISTICS_CALCULATION);
  return (last_run.GetDate() != now.GetDate()) ||
      (::util::Distance(last_run.GetTime(), now.GetTime()) > detail::SESSION_LENGTH);
}

::type::Timestamp ApacheAnalyzerObject::GetCurrentTimestamp() const {
  BOOST_LOG_TRIVIAL(debug) << "apache::analyzer::detail::KnnAnalyzerObject::GetCurrentTimestamp: Function call";
  time_t t = system_interface_->Time(nullptr);
  struct tm *now = system_interface_->LocalTime(&t);

  return ::type::Timestamp::Create(::type::Time::Create(now->tm_hour, now->tm_min, now->tm_sec),
                                   ::type::Date::Create(now->tm_mday, now->tm_mon + 1, now->tm_year + 1900));
}

}

}
