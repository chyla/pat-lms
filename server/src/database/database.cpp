#include "database.h"

#include <utility>
#include <boost/log/trivial.hpp>
#include <slas/type/apache_log_entry.h>

#include "detail/sqlite.h"
#include "src/database/exception/detail/cant_open_database_exception.h"
#include "src/database/exception/detail/cant_close_database_exception.h"
#include "src/database/exception/detail/cant_execute_sql_statement_exception.h"

using namespace std;

namespace database
{

DatabasePtr Database::Create() {
  std::unique_ptr<detail::SQLite> sqlite(new detail::SQLite());
  return Database::Create(std::move(sqlite));
}

DatabasePtr Database::Create(std::unique_ptr<detail::SQLiteInterface> sqlite) {
  DatabasePtr db(new Database(move(sqlite)));
  return db;
}

void Database::Open(sqlite3 *db_handle) {
  db_handle_ = db_handle;
  is_open_ = true;
}

bool Database::IsOpen() const {
  return is_open_;
}

::type::Date Database::GetDateById(type::RowId id) {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::GetDateById: Function call";
  int ret, day, month, year;
  ::type::Date date;
  bool found = false;

  if (is_open_ == false) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::GetDateById: Database is not open";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  string sql =
      "select DAY, MONTH, YEAR from DATE_TABLE "
      "  where"
      "    ID=" + to_string(id) + ";";

  sqlite3_stmt *statement;
  ret = sqlite_interface_->Prepare(db_handle_, sql.c_str(), -1, &statement, nullptr);
  StatementCheckForError(ret, "Prepare error");

  ret = sqlite_interface_->Step(statement);
  StatementCheckForError(ret, "Step error");

  if (ret == SQLITE_ROW) {
    found = true;
    day = sqlite_interface_->ColumnInt(statement, 0);
    month = sqlite_interface_->ColumnInt(statement, 1);
    year = sqlite_interface_->ColumnInt(statement, 2);
    date = ::type::Date::Create(day, month, year);
  }

  ret = sqlite_interface_->Finalize(statement);
  StatementCheckForError(ret, "Finalize error");

  if (!found) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::GetDateById: Date not found";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  return date;
}

bool Database::AddApacheSessionStatistics(const ::apache::type::ApacheSessions &sessions) {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::AddApacheSessionStatistics: Function call";

  if (is_open_ == false) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::AddApacheSessionStatistics: Database is not open.";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  int ret;
  ret = sqlite_interface_->Exec(db_handle_, "begin transaction", nullptr, nullptr, nullptr);
  StatementCheckForError(ret, "Begin transaction error");

  for (const ::apache::type::ApacheSessionEntry &entry : sessions) {
    const char *sql = "insert into APACHE_SESSION_TABLE(AGENT_NAME, VIRTUALHOST, CLIENT_IP, UTC_HOUR, UTC_MINUTE, UTC_SECOND, UTC_DAY, UTC_MONTH,"
        "                                 UTC_YEAR, SESSION_LENGTH, BANDWIDTH_USAGE, REQUESTS_COUNT, ERRORS_COUNT, ERROR_PERCENTAGE, USER_AGENT, CLASSIFICATION)"
        " values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt *statement;
    ret = sqlite_interface_->Prepare(db_handle_, sql, -1, &statement, nullptr);
    StatementCheckForErrorAndRollback(ret, "Prepare insert error");

    ret = sqlite_interface_->BindText(statement, 1, entry.agent_name.c_str(), -1, nullptr);
    StatementCheckForErrorAndRollback(ret, "Bind agent_name error");

    ret = sqlite_interface_->BindText(statement, 2, entry.virtualhost.c_str(), -1, nullptr);
    StatementCheckForErrorAndRollback(ret, "Bind virtualhost error");

    ret = sqlite_interface_->BindText(statement, 3, entry.client_ip.c_str(), -1, nullptr);
    StatementCheckForErrorAndRollback(ret, "Bind client_ip error");

    ret = sqlite_interface_->BindInt(statement, 4, entry.session_start.GetTime().GetHour());
    StatementCheckForErrorAndRollback(ret, "Bind hour error");

    ret = sqlite_interface_->BindInt(statement, 5, entry.session_start.GetTime().GetMinute());
    StatementCheckForErrorAndRollback(ret, "Bind minute error");

    ret = sqlite_interface_->BindInt(statement, 6, entry.session_start.GetTime().GetSecond());
    StatementCheckForErrorAndRollback(ret, "Bind second error");

    ret = sqlite_interface_->BindInt(statement, 7, entry.session_start.GetDate().GetDay());
    StatementCheckForErrorAndRollback(ret, "Bind day error");

    ret = sqlite_interface_->BindInt(statement, 8, entry.session_start.GetDate().GetMonth());
    StatementCheckForErrorAndRollback(ret, "Bind month error");

    ret = sqlite_interface_->BindInt(statement, 9, entry.session_start.GetDate().GetYear());
    StatementCheckForErrorAndRollback(ret, "Bind year error");

    ret = sqlite_interface_->BindInt64(statement, 10, entry.session_length);
    StatementCheckForErrorAndRollback(ret, "Bind session_length error");

    ret = sqlite_interface_->BindInt64(statement, 11, entry.bandwidth_usage);
    StatementCheckForErrorAndRollback(ret, "Bind bandwidth_usage error");

    ret = sqlite_interface_->BindInt64(statement, 12, entry.requests_count);
    StatementCheckForErrorAndRollback(ret, "Bind requests_count error");

    ret = sqlite_interface_->BindInt64(statement, 13, entry.errors_count);
    StatementCheckForErrorAndRollback(ret, "Bind errors_count error");

    ret = sqlite_interface_->BindDouble(statement, 14, entry.error_percentage);
    StatementCheckForErrorAndRollback(ret, "Bind error_percentage error");

    ret = sqlite_interface_->BindText(statement, 15, entry.useragent.c_str(), -1, nullptr);
    StatementCheckForErrorAndRollback(ret, "Bind useragent error");

    ret = sqlite_interface_->BindInt(statement, 16, static_cast<int> (entry.classification));
    StatementCheckForErrorAndRollback(ret, "Bind classification error");

    ret = sqlite_interface_->Step(statement);
    if (ret == SQLITE_BUSY) {
      BOOST_LOG_TRIVIAL(error) << "database::Database::AddApacheSessionStatistics: Step error - SQLite is busy";
      Rollback();
      return false;
    }
    StatementCheckForErrorAndRollback(ret, "Step error");

    ret = sqlite_interface_->Finalize(statement);
    StatementCheckForErrorAndRollback(ret, "Finalize error");
  }

  ret = sqlite_interface_->Exec(db_handle_, "end transaction", nullptr, nullptr, nullptr);
  if (ret == SQLITE_BUSY) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::AddBashLogs: End transaction error - SQLite is busy";
    Rollback();
    return false;
  }
  StatementCheckForErrorAndRollback(ret, "End transaction error");

  return true;
}

long long Database::GetApacheSessionStatisticsCount(const std::string &agent_name, const std::string &virtualhost_name,
                                                    const ::type::Timestamp &from, const ::type::Timestamp &to) {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::GetApacheSessionStatisticsCount: Function call";
  return GetApacheCount("APACHE_SESSION_TABLE", agent_name, virtualhost_name, from, to);
}

::apache::type::ApacheSessions Database::GetApacheSessionStatistics(const std::string &agent_name, const std::string &virtualhost_name,
                                                                    const ::type::Timestamp &from, const ::type::Timestamp &to, unsigned limit, long long offset) {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::GetApacheSessionStatistics: Function call";
  ::apache::type::ApacheSessions sessions;
  int ret, hour, minute, second, day, month, year;

  if (is_open_ == false) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::GetApacheLogs: Database is not open";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  string sql =
      "select ID, AGENT_NAME, VIRTUALHOST, CLIENT_IP, UTC_HOUR, UTC_MINUTE, UTC_SECOND, UTC_DAY, UTC_MONTH, UTC_YEAR, SESSION_LENGTH, BANDWIDTH_USAGE, REQUESTS_COUNT, ERRORS_COUNT, ERROR_PERCENTAGE, USER_AGENT, CLASSIFICATION "
      " from APACHE_SESSION_TABLE "
      "  where"
      "    ("
      "      AGENT_NAME=?"
      "      and"
      "      VIRTUALHOST=?"
      "    )"
      "  and " +
      GetTimeRule(from, to) +
      "   limit " + to_string(limit) + " offset " + to_string(offset) +
      ";";

  sqlite3_stmt *statement;
  ret = sqlite_interface_->Prepare(db_handle_, sql.c_str(), -1, &statement, nullptr);
  StatementCheckForError(ret, "Prepare insert error");

  ret = sqlite_interface_->BindText(statement, 1, agent_name.c_str(), -1, nullptr);
  StatementCheckForError(ret, "Bind useragent error");

  ret = sqlite_interface_->BindText(statement, 2, virtualhost_name.c_str(), -1, nullptr);
  StatementCheckForError(ret, "Bind useragent error");

  do {
    ret = sqlite_interface_->Step(statement);
    StatementCheckForError(ret, "Step error");

    if (ret == SQLITE_ROW) {
      BOOST_LOG_TRIVIAL(debug) << "database::Database::GetApacheLogs: Found new log entry in database";
      ::apache::type::ApacheSessionEntry entry;

      entry.id = sqlite_interface_->ColumnInt64(statement, 0);
      entry.agent_name = TextHelper(sqlite_interface_->ColumnText(statement, 1));
      entry.virtualhost = TextHelper(sqlite_interface_->ColumnText(statement, 2));
      entry.client_ip = TextHelper(sqlite_interface_->ColumnText(statement, 3));

      hour = sqlite_interface_->ColumnInt(statement, 4);
      minute = sqlite_interface_->ColumnInt(statement, 5);
      second = sqlite_interface_->ColumnInt(statement, 6);
      day = sqlite_interface_->ColumnInt(statement, 7);
      month = sqlite_interface_->ColumnInt(statement, 8);
      year = sqlite_interface_->ColumnInt(statement, 9);

      ::type::Timestamp t;
      t.Set(hour, minute, second, day, month, year);

      entry.session_start = t;
      entry.session_length = sqlite_interface_->ColumnInt64(statement, 10);
      entry.bandwidth_usage = sqlite_interface_->ColumnInt64(statement, 11);
      entry.requests_count = sqlite_interface_->ColumnInt64(statement, 12);
      entry.errors_count = sqlite_interface_->ColumnInt64(statement, 13);
      entry.error_percentage = sqlite_interface_->ColumnDouble(statement, 14);
      entry.useragent = TextHelper(sqlite_interface_->ColumnText(statement, 15));
      entry.classification = static_cast< ::database::type::Classification> (sqlite_interface_->ColumnInt(statement, 16));

      sessions.push_back(entry);
    }
  }
  while (ret != SQLITE_DONE);

  ret = sqlite_interface_->Finalize(statement);
  StatementCheckForError(ret, "Finalize error");

  return sessions;
}

void Database::SetApacheSessionAsAnomaly(type::RowIds all, type::RowIds anomalies) {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::SetApacheSessionAsAnomaly: Function call";

  if (is_open_ == false) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::SetApacheSessionAsAnomaly: Database is not open";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  string sql = "begin transaction; ";
  for (auto id : all)
    sql += "update APACHE_SESSION_TABLE set CLASSIFICATION=" + std::to_string(static_cast<int> (::database::type::Classification::NORMAL)) +
    " where ID=" + to_string(id) + "; ";

  for (auto id : anomalies)
    sql += "update APACHE_SESSION_TABLE set CLASSIFICATION=" + std::to_string(static_cast<int> (::database::type::Classification::ANOMALY)) +
    " where ID=" + to_string(id) + "; ";

  sql += "end transaction; ";

  int ret = sqlite_interface_->Exec(db_handle_, sql.c_str(), nullptr, nullptr, nullptr);
  StatementCheckForError(ret, "Exec error");
}

::apache::type::ApacheSessionEntry Database::GetApacheOneSessionStatistic(long long id) {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::GetApacheOneSessionStatistic: Function call";
  ::apache::type::ApacheSessionEntry entry;
  int ret, hour, minute, second, day, month, year;

  if (is_open_ == false) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::GetApacheOneSessionStatistic: Database is not open";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  string sql =
      "select ID, AGENT_NAME, VIRTUALHOST, CLIENT_IP, UTC_HOUR, UTC_MINUTE, UTC_SECOND, UTC_DAY, UTC_MONTH, UTC_YEAR, SESSION_LENGTH, BANDWIDTH_USAGE, REQUESTS_COUNT, ERRORS_COUNT, ERROR_PERCENTAGE, USER_AGENT, CLASSIFICATION "
      " from APACHE_SESSION_TABLE "
      "  where"
      "    ID=" + to_string(id) +
      ";";

  sqlite3_stmt *statement;
  ret = sqlite_interface_->Prepare(db_handle_, sql.c_str(), -1, &statement, nullptr);
  StatementCheckForError(ret, "Prepare insert error");

  ret = sqlite_interface_->Step(statement);
  StatementCheckForError(ret, "Step error");

  if (ret == SQLITE_ROW) {
    BOOST_LOG_TRIVIAL(debug) << "database::Database::GetApacheOneSessionStatistic: Found new log entry in database";

    entry.id = id;
    entry.agent_name = TextHelper(sqlite_interface_->ColumnText(statement, 1));
    entry.virtualhost = TextHelper(sqlite_interface_->ColumnText(statement, 2));
    entry.client_ip = TextHelper(sqlite_interface_->ColumnText(statement, 3));

    hour = sqlite_interface_->ColumnInt(statement, 4);
    minute = sqlite_interface_->ColumnInt(statement, 5);
    second = sqlite_interface_->ColumnInt(statement, 6);
    day = sqlite_interface_->ColumnInt(statement, 7);
    month = sqlite_interface_->ColumnInt(statement, 8);
    year = sqlite_interface_->ColumnInt(statement, 9);

    ::type::Timestamp t;
    t.Set(hour, minute, second, day, month, year);

    entry.session_start = t;
    entry.session_length = sqlite_interface_->ColumnInt64(statement, 10);
    entry.bandwidth_usage = sqlite_interface_->ColumnInt64(statement, 11);
    entry.requests_count = sqlite_interface_->ColumnInt64(statement, 12);
    entry.errors_count = sqlite_interface_->ColumnInt64(statement, 13);
    entry.error_percentage = sqlite_interface_->ColumnDouble(statement, 14);
    entry.useragent = TextHelper(sqlite_interface_->ColumnText(statement, 15));
    entry.classification = static_cast< ::database::type::Classification> (sqlite_interface_->ColumnInt(statement, 16));
  }

  ret = sqlite_interface_->Finalize(statement);
  StatementCheckForError(ret, "Finalize error");

  return entry;
}

void Database::SetApacheAnomalyDetectionConfiguration(const ::apache::type::AnomalyDetectionConfigurationEntry &configuration) {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::SetApacheAnomalyDetectionConfiguration: Function call";
  int ret;

  if (is_open_ == false) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::SetApacheAnomalyDetectionConfiguration: Database is not open.";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  string sql =
      "insert or replace into APACHE_ANOMALY_DETECTION_CONFIGURATION_TABLE (ID, AGENT_NAME, VIRTUALHOST_NAME, BEGIN_DATE_ID, END_DATE_ID) "
      "values ("
      "  ( select ID from  APACHE_ANOMALY_DETECTION_CONFIGURATION_TABLE where AGENT_NAME=? and VIRTUALHOST_NAME=? ), "
      "  ?, "
      "  ?, "
      "  ( select ID from DATE_TABLE where DAY=" + to_string(configuration.begin_date.GetDay()) + " and MONTH=" + to_string(configuration.begin_date.GetMonth()) + " and YEAR=" + to_string(configuration.begin_date.GetYear()) + " ),"
      "  ( select ID from DATE_TABLE where DAY=" + to_string(configuration.end_date.GetDay()) + " and MONTH=" + to_string(configuration.end_date.GetMonth()) + " and YEAR=" + to_string(configuration.end_date.GetYear()) + " )"
      ");";

  sqlite3_stmt *statement;
  ret = sqlite_interface_->Prepare(db_handle_, sql.c_str(), -1, &statement, nullptr);
  StatementCheckForError(ret, "Prepare insert error");

  ret = sqlite_interface_->BindText(statement, 1, configuration.agent_name.c_str(), -1, nullptr);
  StatementCheckForError(ret, "Bind useragent error");

  ret = sqlite_interface_->BindText(statement, 2, configuration.virtualhost_name.c_str(), -1, nullptr);
  StatementCheckForError(ret, "Bind useragent error");

  ret = sqlite_interface_->BindText(statement, 3, configuration.agent_name.c_str(), -1, nullptr);
  StatementCheckForError(ret, "Bind useragent error");

  ret = sqlite_interface_->BindText(statement, 4, configuration.virtualhost_name.c_str(), -1, nullptr);
  StatementCheckForError(ret, "Bind useragent error");

  ret = sqlite_interface_->Step(statement);
  StatementCheckForError(ret, "Step error");

  ret = sqlite_interface_->Finalize(statement);
  StatementCheckForError(ret, "Finalize error");
}

const ::apache::type::AnomalyDetectionConfiguration Database::GetApacheAnomalyDetectionConfiguration() {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::GetApacheAnomalyDetectionConfiguration: Function call";
  int ret;
  type::RowId date_id;
  ::apache::type::AnomalyDetectionConfiguration configuration;

  if (is_open_ == false) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::GetApacheAnomalyDetectionConfiguration: Database is not open.";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  string sql = "select ID, AGENT_NAME, VIRTUALHOST_NAME, BEGIN_DATE_ID, END_DATE_ID from APACHE_ANOMALY_DETECTION_CONFIGURATION_TABLE; ";

  sqlite3_stmt *statement;
  ret = sqlite_interface_->Prepare(db_handle_, sql.c_str(), -1, &statement, nullptr);
  StatementCheckForError(ret, "Prepare insert error");

  do {
    ret = sqlite_interface_->Step(statement);
    StatementCheckForError(ret, "Step error");

    if (ret == SQLITE_ROW) {
      BOOST_LOG_TRIVIAL(debug) << "database::Database::GetApacheAnomalyDetectionConfiguration: Found new entry in database";
      ::apache::type::AnomalyDetectionConfigurationEntry entry;

      entry.id = sqlite_interface_->ColumnInt64(statement, 0);
      entry.agent_name = TextHelper(sqlite_interface_->ColumnText(statement, 1));
      entry.virtualhost_name = TextHelper(sqlite_interface_->ColumnText(statement, 2));

      date_id = sqlite_interface_->ColumnInt64(statement, 3);
      entry.begin_date = GetDateById(date_id);

      date_id = sqlite_interface_->ColumnInt64(statement, 4);
      entry.end_date = GetDateById(date_id);

      configuration.push_back(entry);
    }
  }
  while (ret != SQLITE_DONE);

  ret = sqlite_interface_->Finalize(statement);
  StatementCheckForError(ret, "Finalize error");

  return configuration;
}

type::AgentNames Database::GetApacheAgentNames() {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::GetApacheAgentNames: Function call";

  int ret;
  type::AgentNames names;

  const char *sql = "select DISTINCT AGENT_NAME from APACHE_LOGS_TABLE;";

  ret = sqlite_interface_->Exec(db_handle_, sql, GetApacheAgentNamesCallback, static_cast<void*> (&names), nullptr);
  StatementCheckForError(ret, "Exec error");

  BOOST_LOG_TRIVIAL(debug) << "database::Database::GetApacheAgentNames: Found " << names.size() << " names";

  return names;
}

type::VirtualhostNames Database::GetApacheVirtualhostNames(std::string agent_name) {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::GetApacheVirtualhostNames: Function call";

  type::VirtualhostNames names;
  int ret;
  const char *sql = "select distinct VIRTUALHOST from APACHE_LOGS_TABLE where AGENT_NAME = ?;";
  sqlite3_stmt *statement;

  ret = sqlite_interface_->Prepare(db_handle_, sql, -1, &statement, nullptr);
  StatementCheckForError(ret, "Prepare insert error");

  ret = sqlite_interface_->BindText(statement, 1, agent_name.c_str(), -1, nullptr);
  StatementCheckForError(ret, "Bind useragent error");

  do {
    ret = sqlite_interface_->Step(statement);
    StatementCheckForError(ret, "Step error");

    if (ret == SQLITE_ROW) {
      const char *pname = reinterpret_cast<const char*> (sqlite_interface_->ColumnText(statement, 0));
      if (pname)
        names.push_back(pname);
    }
  }
  while (ret != SQLITE_DONE);

  ret = sqlite_interface_->Finalize(statement);
  StatementCheckForError(ret, "Finalize error");

  return names;
}

Database::Database(std::unique_ptr<database::detail::SQLiteInterface> sqlite)
: is_open_(false),
db_handle_(nullptr),
sqlite_interface_(std::move(sqlite)) {
}

void Database::StatementCheckForError(int return_value, const char *description) {
  if (return_value != SQLITE_OK && return_value != SQLITE_DONE && return_value != SQLITE_ROW) {
    BOOST_LOG_TRIVIAL(error) << "database::Database: " << description << ": " << return_value;
    throw exception::detail::CantExecuteSqlStatementException();
  }
}

void Database::StatementCheckForErrorAndRollback(int return_value, const char *description) {
  if (return_value != SQLITE_OK && return_value != SQLITE_DONE && return_value != SQLITE_ROW) {
    BOOST_LOG_TRIVIAL(error) << "database::Database: " << description << ": " << return_value;
    Rollback();
    throw exception::detail::CantExecuteSqlStatementException();
  }
}

void Database::Rollback() {
  int ret = sqlite_interface_->Exec(db_handle_, "rollback", nullptr, nullptr, nullptr);
  if (ret != SQLITE_OK) {
    BOOST_LOG_TRIVIAL(error) << "database::Database: Rollback error: " << ret;
    throw exception::detail::CantExecuteSqlStatementException();
  }
}

int Database::GetApacheAgentNamesCallback(void *names_vptr, int argc, char **argv, char **azColName) {
  type::AgentNames *names_ptr = static_cast<type::AgentNames*> (names_vptr);

  for (int i = 0; i < argc; i++) {
    if (argv[i] != nullptr) {
      names_ptr->push_back(argv[i]);
    }
  }

  return 0;
}

string Database::GetTimeRule(const ::type::Timestamp &from, const ::type::Timestamp &to) const {
  string from_day = to_string(from.GetDate().GetDay()),
      from_month = to_string(from.GetDate().GetMonth()),
      from_year = to_string(from.GetDate().GetYear()),
      from_hour = to_string(from.GetTime().GetHour()),
      from_minute = to_string(from.GetTime().GetMinute()),
      from_second = to_string(from.GetTime().GetSecond()),
      to_day = to_string(to.GetDate().GetDay()),
      to_month = to_string(to.GetDate().GetMonth()),
      to_year = to_string(to.GetDate().GetYear()),
      to_hour = to_string(to.GetTime().GetHour()),
      to_minute = to_string(to.GetTime().GetMinute()),
      to_second = to_string(to.GetTime().GetSecond());

  string rule =
      "  ("
      "    ("
      "      (UTC_YEAR > " + from_year + ")"
      "      or (UTC_YEAR = " + from_year + " and UTC_MONTH > " + from_month + ")"
      "      or (UTC_YEAR = " + from_year + " and UTC_MONTH = " + from_month + " and UTC_DAY > " + from_day + ")"
      "      or (UTC_YEAR = " + from_year + " and UTC_MONTH = " + from_month + " and UTC_DAY = " + from_day + " and UTC_HOUR > " + from_hour + ")"
      "      or (UTC_YEAR = " + from_year + " and UTC_MONTH = " + from_month + " and UTC_DAY = " + from_day + " and UTC_HOUR = " + from_hour + " and UTC_MINUTE > " + from_minute + ")"
      "      or (UTC_YEAR = " + from_year + " and UTC_MONTH = " + from_month + " and UTC_DAY = " + from_day + " and UTC_HOUR = " + from_hour + " and UTC_MINUTE = " + from_minute + " and UTC_SECOND >= " + from_second + ")"
      "    )"
      "  and"
      "    ("
      "      (UTC_YEAR < " + to_year + ")"
      "      or (UTC_YEAR = " + to_year + " and UTC_MONTH < " + to_month + ")"
      "      or (UTC_YEAR = " + to_year + " and UTC_MONTH = " + to_month + " and UTC_DAY < " + to_day + ")"
      "      or (UTC_YEAR = " + to_year + " and UTC_MONTH = " + to_month + " and UTC_DAY = " + to_day + " and UTC_HOUR < " + to_hour + ")"
      "      or (UTC_YEAR = " + to_year + " and UTC_MONTH = " + to_month + " and UTC_DAY = " + to_day + " and UTC_HOUR = " + to_hour + " and UTC_MINUTE < " + to_minute + ")"
      "      or (UTC_YEAR = " + to_year + " and UTC_MONTH = " + to_month + " and UTC_DAY = " + to_day + " and UTC_HOUR = " + to_hour + " and UTC_MINUTE = " + to_minute + " and UTC_SECOND <= " + to_second + ")"
      "    )"
      "  )"
      ;

  return rule;
}

std::string Database::TextHelper(unsigned const char *text) const {
  std::string result;

  const char *cctext = reinterpret_cast<const char*> (text);
  if (cctext)
    result = cctext;

  return result;
}

long long Database::GetApacheCount(const std::string &table, const std::string &agent_name,
                                   const std::string &virtualhost_name, const ::type::Timestamp &from,
                                   const ::type::Timestamp &to) {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::GetApacheCount: Function call";
  int ret;
  long long count = 0;

  if (is_open_ == false) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::GetApacheCount: Database is not open";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  string sql =
      "select count(*) from " + table +
      "  where"
      "    ("
      "      AGENT_NAME=?"
      "      and"
      "      VIRTUALHOST=?"
      "    )"
      "  and" +
      GetTimeRule(from, to) +
      ";";

  sqlite3_stmt *statement;
  ret = sqlite_interface_->Prepare(db_handle_, sql.c_str(), -1, &statement, nullptr);
  StatementCheckForError(ret, "Prepare insert error");

  ret = sqlite_interface_->BindText(statement, 1, agent_name.c_str(), -1, nullptr);
  StatementCheckForError(ret, "Bind useragent error");

  ret = sqlite_interface_->BindText(statement, 2, virtualhost_name.c_str(), -1, nullptr);
  StatementCheckForError(ret, "Bind useragent error");

  ret = sqlite_interface_->Step(statement);
  StatementCheckForError(ret, "Step error");

  count = sqlite_interface_->ColumnInt64(statement, 0);

  ret = sqlite_interface_->Finalize(statement);
  StatementCheckForError(ret, "Finalize error");

  return count;
}

}
