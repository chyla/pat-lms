#include <memory>
#include <utility>

#include <slas/type/timestamp.h>
#include <slas/type/bash_log_entry.h>
#include <slas/type/apache_log_entry.h>

#include "src/database/database.h"
#include "src/database/exception/detail/cant_open_database_exception.h"
#include "src/database/exception/detail/cant_execute_sql_statement_exception.h"

#include "mock/database/sqlite.h"
#include "src/apache/type/anomaly_detection_configuration_entry.h"

using std::unique_ptr;
using std::move;
using namespace testing;
using namespace database;

#define DB_HANDLE_EXAMPLE_PTR_VALUE (reinterpret_cast<sqlite3*>(0x000001))
#define DB_STATEMENT_EXAMPLE_PTR_VALUE (reinterpret_cast<sqlite3_stmt*>(0x000002))

::type::Timestamp MY_DEFAULT_TIME1() {
  ::type::Timestamp t;
  t.Set(21, 12, 3, 22, 4, 2011);
  return t;
}

::type::Timestamp MY_DEFAULT_TIME2() {
  ::type::Timestamp t;
  t.Set(22, 13, 4, 23, 5, 2012);
  return t;
}

void MY_EXPECT_DEFAULT_TIME1(unique_ptr<mock::database::SQLite> &sqlite_mock) {
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, 21)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, 12)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 3)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 22)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 4)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7, 2011)).WillOnce(Return(SQLITE_OK));
}

void MY_EXPECT_DEFAULT_TIME2(unique_ptr<mock::database::SQLite> &sqlite_mock) {
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, 22)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, 13)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 4)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 23)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 5)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7, 2012)).WillOnce(Return(SQLITE_OK));
}

void MY_EXPECT_PREPARE(unique_ptr<mock::database::SQLite> &sqlite_mock, int times = 1, int return_value = SQLITE_OK) {
  EXPECT_CALL(*sqlite_mock, Prepare(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), -1, NotNull(), nullptr))
      .Times(times)
      .WillRepeatedly(
                      DoAll(SetArgPointee<3>(DB_STATEMENT_EXAMPLE_PTR_VALUE),
                            Return(return_value)
                            )
                      );
}

TEST(DatabaseTest, ConstructorTest) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());
  DatabasePtr database = Database::Create(move(sqlite_mock));

  EXPECT_FALSE(database->IsOpen());
}

TEST(DatabaseTest, AddApacheSessionStatistics_WithEmptySessionsList) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;

  EXPECT_TRUE(database->AddApacheSessionStatistics(sessions));
}

TEST(DatabaseTest, AddApacheSessionStatistics_WithOneEntry) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 21)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 12)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 3)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7, 22)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8, 4)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 9, 2011)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 10, 23)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 11, 941)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 12, 13)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 13, 10)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindDouble(DB_STATEMENT_EXAMPLE_PTR_VALUE, 14, 44)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 15, StrEq("User-Agent Example"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 16, 2)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_DONE));
  EXPECT_CALL(*sqlite_mock, Finalize(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_OK));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example", ::database::type::Classification::ANOMALY});

  EXPECT_TRUE(database->AddApacheSessionStatistics(sessions));
}

TEST(DatabaseTest, AddApacheSessionStatistics_WithTwoEntries) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock, 2);

  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 21)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 12)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 3)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7, 22)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8, 4)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 9, 2011)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 10, 23)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 11, 941)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 12, 13)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 13, 10)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindDouble(DB_STATEMENT_EXAMPLE_PTR_VALUE, 14, 44)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 15, StrEq("User-Agent Example"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 16, 1)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname2"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh2"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.12"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 22)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 13)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 4)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7, 23)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8, 5)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 9, 2012)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 10, 232)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 11, 9412)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 12, 132)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 13, 20)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindDouble(DB_STATEMENT_EXAMPLE_PTR_VALUE, 14, 442)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 15, StrEq("User-Agent Example 2"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 16, 2)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).Times(2).WillRepeatedly(Return(SQLITE_DONE));
  EXPECT_CALL(*sqlite_mock, Finalize(DB_STATEMENT_EXAMPLE_PTR_VALUE)).Times(2).WillRepeatedly(Return(SQLITE_OK));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example", ::database::type::Classification::NORMAL});
  sessions.push_back({0, "agentname2", "vh2", "127.0.0.12", MY_DEFAULT_TIME2(), 232, 9412, 132, 20, 442, "User-Agent Example 2", ::database::type::Classification::ANOMALY});

  EXPECT_TRUE(database->AddApacheSessionStatistics(sessions));
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenPrepareFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock, 1, SQLITE_NOMEM);

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example"});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenFinalizeFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 21)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 12)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 3)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7, 22)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8, 4)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 9, 2011)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 10, 23)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 11, 941)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 12, 13)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 13, 10)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindDouble(DB_STATEMENT_EXAMPLE_PTR_VALUE, 14, 44)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 15, StrEq("User-Agent Example"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 16, 1)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_DONE));
  EXPECT_CALL(*sqlite_mock, Finalize(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example", ::database::type::Classification::NORMAL});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenStepFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 21)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 12)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 3)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7, 22)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8, 4)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 9, 2011)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 10, 23)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 11, 941)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 12, 13)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 13, 10)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindDouble(DB_STATEMENT_EXAMPLE_PTR_VALUE, 14, 44)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 15, StrEq("User-Agent Example"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 16, 2)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example", ::database::type::Classification::ANOMALY});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, AddApacheSessionStatistics_StepWhenDatabaseIsBusy) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 21)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 12)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 3)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7, 22)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8, 4)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 9, 2011)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 10, 23)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 11, 941)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 12, 13)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 13, 10)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindDouble(DB_STATEMENT_EXAMPLE_PTR_VALUE, 14, 44)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 15, StrEq("User-Agent Example"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 16, 2)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_BUSY));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example", ::database::type::Classification::ANOMALY});

  EXPECT_FALSE(database->AddApacheSessionStatistics(sessions));
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenBindUserAgentFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 21)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 12)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 3)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7, 22)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8, 4)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 9, 2011)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 10, 23)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 11, 941)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 12, 13)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 13, 10)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindDouble(DB_STATEMENT_EXAMPLE_PTR_VALUE, 14, 44)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 15, StrEq("User-Agent Example"), -1, nullptr)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example"});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenBindErrorPercentageFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 21)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 12)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 3)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7, 22)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8, 4)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 9, 2011)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 10, 23)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 11, 941)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 12, 13)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 13, 10)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindDouble(DB_STATEMENT_EXAMPLE_PTR_VALUE, 14, 44)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example"});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenBindRequestsCountFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 21)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 12)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 3)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7, 22)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8, 4)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 9, 2011)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 10, 23)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 11, 941)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 12, 13)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example"});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenBindBandwidthUsageFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 21)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 12)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 3)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7, 22)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8, 4)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 9, 2011)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 10, 23)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 11, 941)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example"});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenBindSessionLengthFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 21)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 12)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 3)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7, 22)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8, 4)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 9, 2011)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 10, 23)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example"});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenBindYearFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 21)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 12)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 3)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7, 22)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8, 4)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 9, 2011)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example"});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenBindMonthFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 21)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 12)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 3)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7, 22)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8, 4)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example"});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenBindDayFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 21)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 12)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 3)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7, 22)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example"});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenBindSecondFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 21)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 12)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6, 3)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example"});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenBindMinuteFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 21)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5, 12)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example"});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenBindHourFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, BindInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4, 21)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example"});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenBindClientIPFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3, StrEq("127.0.0.1"), -1, nullptr)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example"});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenBindVirtualhostFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example"});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, AddApacheSessionStatistics_WhenBindAgentNameFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).Times(2).WillRepeatedly(Return(SQLITE_OK));
  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::apache::type::ApacheSessions sessions;
  sessions.push_back({0, "agentname", "vh1", "127.0.0.1", MY_DEFAULT_TIME1(), 23, 941, 13, 10, 44, "User-Agent Example"});

  EXPECT_THROW(database->AddApacheSessionStatistics(sessions), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheAgentNames) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), NotNull(), NotNull(), IsNull())).Times(1).WillRepeatedly(Return(SQLITE_OK));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  auto names = database->GetApacheAgentNames();
  EXPECT_EQ(static_cast<unsigned> (0), names.size());
}

TEST(DatabaseTest, GetApacheAgentNames_WhenExecFail) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), NotNull(), NotNull(), IsNull())).Times(1).WillRepeatedly(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  EXPECT_THROW(database->GetApacheAgentNames(), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheVirtualhostNames) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname2"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).Times(2).WillOnce(Return(SQLITE_ROW)).WillOnce(Return(SQLITE_DONE));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 0)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("virtualhost2")));
  EXPECT_CALL(*sqlite_mock, Finalize(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_OK));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  auto names = database->GetApacheVirtualhostNames("agentname2");
  EXPECT_EQ(static_cast<unsigned> (1), names.size());
  EXPECT_STREQ("virtualhost2", names.at(0).c_str());
}

TEST(DatabaseTest, GetApacheVirtualhostNames_WhenTwoNamesAreAvailable) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname2"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).Times(3).WillOnce(Return(SQLITE_ROW)).WillOnce(Return(SQLITE_ROW)).WillOnce(Return(SQLITE_DONE));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 0)).Times(2)
      .WillOnce(Return(reinterpret_cast<unsigned const char *> ("virtualhost2")))
      .WillOnce(Return(reinterpret_cast<unsigned const char *> ("virtualhost1")));
  EXPECT_CALL(*sqlite_mock, Finalize(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_OK));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  auto names = database->GetApacheVirtualhostNames("agentname2");
  EXPECT_EQ(static_cast<unsigned> (2), names.size());
  EXPECT_STREQ("virtualhost2", names.at(0).c_str());
  EXPECT_STREQ("virtualhost1", names.at(1).c_str());
}

TEST(DatabaseTest, GetApacheVirtualhostNames_WhenColumnTextReturnsNull) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname2"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).Times(3).WillOnce(Return(SQLITE_ROW)).WillOnce(Return(SQLITE_ROW)).WillOnce(Return(SQLITE_DONE));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 0)).Times(2)
      .WillOnce(Return(reinterpret_cast<unsigned const char *> ("virtualhost2")))
      .WillOnce(Return(nullptr));
  EXPECT_CALL(*sqlite_mock, Finalize(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_OK));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  auto names = database->GetApacheVirtualhostNames("agentname2");
  EXPECT_EQ(static_cast<unsigned> (1), names.size());
  EXPECT_STREQ("virtualhost2", names.at(0).c_str());
}

TEST(DatabaseTest, GetApacheVirtualhostNames_WhenFinalizeFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname2"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).Times(2).WillOnce(Return(SQLITE_ROW)).WillOnce(Return(SQLITE_DONE));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 0)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("virtualhost2")));
  EXPECT_CALL(*sqlite_mock, Finalize(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  EXPECT_THROW(database->GetApacheVirtualhostNames("agentname2"), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheVirtualhostNames_WhenStepFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname2"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  EXPECT_THROW(database->GetApacheVirtualhostNames("agentname2"), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheVirtualhostNames_WhenBindAgentNameFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname2"), -1, nullptr)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  EXPECT_THROW(database->GetApacheVirtualhostNames("agentname2"), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheVirtualhostNames_WhenPrepareFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock, 1, SQLITE_NOMEM);

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  EXPECT_THROW(database->GetApacheVirtualhostNames("agentname2"), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheSessionStatisticsCount) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock, 1);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_DONE));
  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 0)).WillOnce(Return(43));
  EXPECT_CALL(*sqlite_mock, Finalize(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_OK));

  ::type::Timestamp from, to;
  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  EXPECT_EQ(43, database->GetApacheSessionStatisticsCount("agentname", "vh1", from, to));
}

TEST(DatabaseTest, GetApacheSessionStatisticsCount_WhenFinalizeFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock, 1);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_DONE));
  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 0)).WillOnce(Return(43));
  EXPECT_CALL(*sqlite_mock, Finalize(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_NOMEM));

  ::type::Timestamp from, to;
  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  EXPECT_THROW(database->GetApacheSessionStatisticsCount("agentname", "vh1", from, to), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheSessionStatisticsCount_WhenStepFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock, 1);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_NOMEM));

  ::type::Timestamp from, to;
  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  EXPECT_THROW(database->GetApacheSessionStatisticsCount("agentname", "vh1", from, to), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheSessionStatisticsCount_WhenBindVirtualhostFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock, 1);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_NOMEM));

  ::type::Timestamp from, to;
  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  EXPECT_THROW(database->GetApacheSessionStatisticsCount("agentname", "vh1", from, to), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheSessionStatisticsCount_WhenBindAgentNameFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock, 1);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_NOMEM));

  ::type::Timestamp from, to;
  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  EXPECT_THROW(database->GetApacheSessionStatisticsCount("agentname", "vh1", from, to), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheSessionStatisticsCount_WhenPrepareFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock, 1, SQLITE_NOMEM);

  ::type::Timestamp from, to;
  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  EXPECT_THROW(database->GetApacheSessionStatisticsCount("agentname", "vh1", from, to), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheSessionStatistics) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).Times(2).WillOnce(Return(SQLITE_ROW)).WillOnce(Return(SQLITE_DONE));

  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 0)).WillOnce(Return(1));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("agentname")));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("vh1")));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("127.0.0.1")));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4)).WillOnce(Return(10));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5)).WillOnce(Return(6));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6)).WillOnce(Return(7));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7)).WillOnce(Return(2));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8)).WillOnce(Return(9));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 9)).WillOnce(Return(2013));

  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 10)).WillOnce(Return(2));
  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 11)).WillOnce(Return(3));
  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 12)).WillOnce(Return(4));
  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 13)).WillOnce(Return(10));
  EXPECT_CALL(*sqlite_mock, ColumnDouble(DB_STATEMENT_EXAMPLE_PTR_VALUE, 14)).WillOnce(Return(5.2));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 15)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("useragent")));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 16)).WillOnce(Return(1));

  EXPECT_CALL(*sqlite_mock, Finalize(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_OK));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  ::type::Timestamp from, to;
  from.Set(10, 0, 0, 1, 1, 2015);
  to.Set(10, 0, 0, 1, 1, 2017);
  auto names = database->GetApacheSessionStatistics("agentname", "vh1", from, to, 100, 0);
  EXPECT_EQ(static_cast<unsigned> (1), names.size());

  auto session = names.at(0);
  EXPECT_EQ(1, session.id);
  EXPECT_STREQ("agentname", session.agent_name.c_str());
  EXPECT_STREQ("vh1", session.virtualhost.c_str());
  EXPECT_STREQ("127.0.0.1", session.client_ip.c_str());
  EXPECT_EQ(10, session.session_start.GetTime().GetHour());
  EXPECT_EQ(6, session.session_start.GetTime().GetMinute());
  EXPECT_EQ(7, session.session_start.GetTime().GetSecond());
  EXPECT_EQ(2, session.session_start.GetDate().GetDay());
  EXPECT_EQ(9, session.session_start.GetDate().GetMonth());
  EXPECT_EQ(2013, session.session_start.GetDate().GetYear());
  EXPECT_EQ(2, session.session_length);
  EXPECT_EQ(3, session.bandwidth_usage);
  EXPECT_EQ(4, session.requests_count);
  EXPECT_EQ(10, session.errors_count);
  EXPECT_DOUBLE_EQ(5.2, session.error_percentage);
  EXPECT_STREQ("useragent", session.useragent.c_str());
  EXPECT_EQ(::database::type::Classification::NORMAL, session.classification);
}

TEST(DatabaseTest, GetApacheSessionStatistics_WhenFinalizeFail) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).Times(2).WillOnce(Return(SQLITE_ROW)).WillOnce(Return(SQLITE_DONE));

  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 0)).WillOnce(Return(1));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("agentname")));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("vh1")));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("127.0.0.1")));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4)).WillOnce(Return(10));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5)).WillOnce(Return(6));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6)).WillOnce(Return(7));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7)).WillOnce(Return(2));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8)).WillOnce(Return(9));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 9)).WillOnce(Return(2013));

  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 10)).WillOnce(Return(2));
  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 11)).WillOnce(Return(3));
  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 12)).WillOnce(Return(4));
  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 13)).WillOnce(Return(10));
  EXPECT_CALL(*sqlite_mock, ColumnDouble(DB_STATEMENT_EXAMPLE_PTR_VALUE, 14)).WillOnce(Return(5));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 15)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("useragent")));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 16)).WillOnce(Return(1));

  EXPECT_CALL(*sqlite_mock, Finalize(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  ::type::Timestamp from, to;
  from.Set(10, 0, 0, 1, 1, 2015);
  to.Set(10, 0, 0, 1, 1, 2017);
  EXPECT_THROW(database->GetApacheSessionStatistics("agentname", "vh1", from, to, 100, 0), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheSessionStatistics_WhenStepFail) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_OK));

  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  ::type::Timestamp from, to;
  from.Set(10, 0, 0, 1, 1, 2015);
  to.Set(10, 0, 0, 1, 1, 2017);
  EXPECT_THROW(database->GetApacheSessionStatistics("agentname", "vh1", from, to, 100, 0), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheSessionStatistics_WhenBindVirtualhostFail) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_OK));
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2, StrEq("vh1"), -1, nullptr)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  ::type::Timestamp from, to;
  from.Set(10, 0, 0, 1, 1, 2015);
  to.Set(10, 0, 0, 1, 1, 2017);
  EXPECT_THROW(database->GetApacheSessionStatistics("agentname", "vh1", from, to, 100, 0), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheSessionStatistics_WhenBindAgentNameFail) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, BindText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1, StrEq("agentname"), -1, nullptr)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  ::type::Timestamp from, to;
  from.Set(10, 0, 0, 1, 1, 2015);
  to.Set(10, 0, 0, 1, 1, 2017);
  EXPECT_THROW(database->GetApacheSessionStatistics("agentname", "vh1", from, to, 100, 0), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheSessionStatistics_WhenPrepareFail) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock, 1, SQLITE_NOMEM);

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  ::type::Timestamp from, to;
  from.Set(10, 0, 0, 1, 1, 2015);
  to.Set(10, 0, 0, 1, 1, 2017);
  EXPECT_THROW(database->GetApacheSessionStatistics("agentname", "vh1", from, to, 100, 0), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheOneSessionStatistic) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).Times(1).WillOnce(Return(SQLITE_ROW));

  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("agentname")));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("vh1")));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("127.0.0.1")));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4)).WillOnce(Return(10));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5)).WillOnce(Return(6));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6)).WillOnce(Return(7));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7)).WillOnce(Return(2));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8)).WillOnce(Return(9));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 9)).WillOnce(Return(2013));

  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 10)).WillOnce(Return(2));
  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 11)).WillOnce(Return(3));
  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 12)).WillOnce(Return(4));
  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 13)).WillOnce(Return(10));
  EXPECT_CALL(*sqlite_mock, ColumnDouble(DB_STATEMENT_EXAMPLE_PTR_VALUE, 14)).WillOnce(Return(5.5));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 15)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("useragent")));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 16)).WillOnce(Return(1));

  EXPECT_CALL(*sqlite_mock, Finalize(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_OK));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  ::type::Timestamp from, to;
  from.Set(10, 0, 0, 1, 1, 2015);
  to.Set(10, 0, 0, 1, 1, 2017);
  auto session = database->GetApacheOneSessionStatistic(1);
  EXPECT_EQ(1, session.id);
  EXPECT_STREQ("agentname", session.agent_name.c_str());
  EXPECT_STREQ("vh1", session.virtualhost.c_str());
  EXPECT_STREQ("127.0.0.1", session.client_ip.c_str());
  EXPECT_EQ(10, session.session_start.GetTime().GetHour());
  EXPECT_EQ(6, session.session_start.GetTime().GetMinute());
  EXPECT_EQ(7, session.session_start.GetTime().GetSecond());
  EXPECT_EQ(2, session.session_start.GetDate().GetDay());
  EXPECT_EQ(9, session.session_start.GetDate().GetMonth());
  EXPECT_EQ(2013, session.session_start.GetDate().GetYear());
  EXPECT_EQ(2, session.session_length);
  EXPECT_EQ(3, session.bandwidth_usage);
  EXPECT_EQ(4, session.requests_count);
  EXPECT_DOUBLE_EQ(5.5, session.error_percentage);
  EXPECT_STREQ("useragent", session.useragent.c_str());
  EXPECT_EQ(::database::type::Classification::NORMAL, session.classification);
}

TEST(DatabaseTest, GetApacheOneSessionStatistic_WhenFinalizeFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).Times(1).WillOnce(Return(SQLITE_ROW));

  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 1)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("agentname")));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 2)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("vh1")));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 3)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("127.0.0.1")));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 4)).WillOnce(Return(10));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 5)).WillOnce(Return(6));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 6)).WillOnce(Return(7));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 7)).WillOnce(Return(2));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 8)).WillOnce(Return(9));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 9)).WillOnce(Return(2013));

  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 10)).WillOnce(Return(2));
  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 11)).WillOnce(Return(3));
  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 12)).WillOnce(Return(3));
  EXPECT_CALL(*sqlite_mock, ColumnInt64(DB_STATEMENT_EXAMPLE_PTR_VALUE, 13)).WillOnce(Return(4));
  EXPECT_CALL(*sqlite_mock, ColumnDouble(DB_STATEMENT_EXAMPLE_PTR_VALUE, 14)).WillOnce(Return(5));
  EXPECT_CALL(*sqlite_mock, ColumnText(DB_STATEMENT_EXAMPLE_PTR_VALUE, 15)).WillOnce(Return(reinterpret_cast<unsigned const char *> ("useragent")));
  EXPECT_CALL(*sqlite_mock, ColumnInt(DB_STATEMENT_EXAMPLE_PTR_VALUE, 16)).WillOnce(Return(1));

  EXPECT_CALL(*sqlite_mock, Finalize(DB_STATEMENT_EXAMPLE_PTR_VALUE)).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  ::type::Timestamp from, to;
  from.Set(10, 0, 0, 1, 1, 2015);
  to.Set(10, 0, 0, 1, 1, 2017);
  EXPECT_THROW(database->GetApacheOneSessionStatistic(1), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheOneSessionStatistic_WhenStepFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock);
  EXPECT_CALL(*sqlite_mock, Step(DB_STATEMENT_EXAMPLE_PTR_VALUE)).Times(1).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  ::type::Timestamp from, to;
  from.Set(10, 0, 0, 1, 1, 2015);
  to.Set(10, 0, 0, 1, 1, 2017);
  EXPECT_THROW(database->GetApacheOneSessionStatistic(1), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, GetApacheOneSessionStatistic_WhenPrepareFailed) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  MY_EXPECT_PREPARE(sqlite_mock, 1, SQLITE_NOMEM);

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);

  ::type::Timestamp from, to;
  from.Set(10, 0, 0, 1, 1, 2015);
  to.Set(10, 0, 0, 1, 1, 2017);
  EXPECT_THROW(database->GetApacheOneSessionStatistic(1), database::exception::detail::CantExecuteSqlStatementException);
}

TEST(DatabaseTest, SetApacheSessionAsAnomaly) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).WillOnce(Return(SQLITE_OK));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::database::type::RowIds ids{1};
  database->SetApacheSessionAsAnomaly(ids, ids);
}

TEST(DatabaseTest, SetApacheSessionAsAnomaly_WhenExecFail) {
  unique_ptr<mock::database::SQLite> sqlite_mock(new mock::database::SQLite());

  EXPECT_CALL(*sqlite_mock, Exec(DB_HANDLE_EXAMPLE_PTR_VALUE, NotNull(), IsNull(), IsNull(), IsNull())).WillOnce(Return(SQLITE_NOMEM));

  DatabasePtr database = Database::Create(move(sqlite_mock));
  database->Open(DB_HANDLE_EXAMPLE_PTR_VALUE);
  ::database::type::RowIds ids{1};
  EXPECT_THROW(database->SetApacheSessionAsAnomaly(ids, ids), database::exception::detail::CantExecuteSqlStatementException);
}
