#ifndef SRC_APACHE_WEB_COMMAND_EXECUTOR_OBJECT_H
#define SRC_APACHE_WEB_COMMAND_EXECUTOR_OBJECT_H

#include "src/database/database.h"
#include "src/web/detail/command_executor_object.h"

#include <memory>

namespace apache
{

namespace web
{

class CommandExecutorObject;
typedef std::shared_ptr<CommandExecutorObject> CommandExecutorObjectPtr;

class CommandExecutorObject : public ::web::detail::CommandExecutorObject {
 public:
  virtual ~CommandExecutorObject() = default;

  static CommandExecutorObjectPtr Create(::database::DatabasePtr database);

  std::string Execute(std::string command);

  bool IsCommandSupported(std::string command);

 private:
  CommandExecutorObject(::database::DatabasePtr database);

  ::database::DatabasePtr database_;

  std::string GetHostnames();
  std::string GetVirtualhostsNames(const std::string &agent_name);
};

}

}

#endif /* PROGRAM_OPTIONS_COMMAND_EXECUTOR_OBJECT_H */