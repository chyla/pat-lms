/*
 * Copyright 2016 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "bash_analyzer_object.h"

#include "detail/daily_user_statistics_creator.h"
#include "detail/network_trainer/network_trainer.h"
#include "detail/classificator/classificator.h"
#include "detail/system.h"

#include <boost/log/trivial.hpp>

namespace bash
{

namespace analyzer
{

BashAnalyzerObjectPtr BashAnalyzerObject::Create(::bash::database::detail::DatabaseFunctionsInterfacePtr database_functions,
                                                 ::database::detail::GeneralDatabaseFunctionsInterfacePtr general_database_functions,
                                                 ::bash::domain::detail::ScriptsInterfacePtr scripts_interface,
                                                 const std::string &neural_network_data_directory) {
  auto dusc = detail::DailyUserStatisticsCreator::Create(database_functions, general_database_functions);
  auto nt = detail::network_trainer::NetworkTrainer::Create(database_functions, general_database_functions, neural_network_data_directory);
  auto cr = detail::classificator::Classificator::Create(database_functions, general_database_functions, neural_network_data_directory);
  auto system = detail::System::Create();

  return BashAnalyzerObjectPtr(new BashAnalyzerObject(dusc, nt, cr, scripts_interface, system));
}

BashAnalyzerObjectPtr BashAnalyzerObject::Create(::bash::database::detail::DatabaseFunctionsInterfacePtr database_functions,
                                                 ::database::detail::GeneralDatabaseFunctionsInterfacePtr general_database_functions,
                                                 ::bash::domain::detail::ScriptsInterfacePtr scripts_interface,
                                                 detail::SystemInterfacePtr system_interface,
                                                 const std::string &neural_network_data_directory) {
  auto dusc = detail::DailyUserStatisticsCreator::Create(database_functions, general_database_functions);
  auto nt = detail::network_trainer::NetworkTrainer::Create(database_functions, general_database_functions, neural_network_data_directory);
  auto cr = detail::classificator::Classificator::Create(database_functions, general_database_functions, neural_network_data_directory);

  return BashAnalyzerObjectPtr(new BashAnalyzerObject(dusc, nt, cr, scripts_interface, system_interface));
}

void BashAnalyzerObject::Analyze() {
  BOOST_LOG_TRIVIAL(debug) << "bash::analyzer::BashAnalyzerObject::Analyze: Function call";

  auto today = GetCurrentDate();

  scripts_interface_->CreateDailySystemStatistics();
  daily_user_statistics_creator_->CreateStatistics(today);
  network_trainer_->Train();
  classificator_->Analyze();
}

BashAnalyzerObject::BashAnalyzerObject(detail::DailyUserStatisticsCreatorInterfacePtr daily_user_statistics_creator,
                                       detail::network_trainer::NetworkTrainerInterfacePtr network_trainer,
                                       detail::classificator::ClassificatorInterfacePtr classificator,
                                       ::bash::domain::detail::ScriptsInterfacePtr scripts_interface,
                                       detail::SystemInterfacePtr system_interface) :
daily_user_statistics_creator_(daily_user_statistics_creator),
network_trainer_(network_trainer),
classificator_(classificator),
scripts_interface_(scripts_interface),
system_interface_(system_interface) {
}

::type::Date BashAnalyzerObject::GetCurrentDate() const {
  BOOST_LOG_TRIVIAL(debug) << "bash::analyzer::BashAnalyzerObject::GetCurrentDate: Function call";
  time_t t = system_interface_->Time(nullptr);
  struct tm *now = system_interface_->LocalTime(&t);

  return ::type::Date::Create(now->tm_mday, now->tm_mon + 1, now->tm_year + 1900);
}

}

}
