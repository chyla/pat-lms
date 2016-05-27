#ifndef SRC_PROGRAM_OPTIONS_WEB_COMMAND_EXECUTOR_OBJECT_H
#define SRC_PROGRAM_OPTIONS_WEB_COMMAND_EXECUTOR_OBJECT_H

#include <memory>

#include "src/program_options/options.h"
#include "src/web/detail/command_executor_object.h"

namespace program_options
{

namespace web
{

class CommandExecutorObject;
typedef std::shared_ptr<CommandExecutorObject> CommandExecutorObjectPtr;

class CommandExecutorObject : public ::web::detail::CommandExecutorObject {
 public:
  virtual ~CommandExecutorObject() = default;

  static CommandExecutorObjectPtr Create(const Options &options);

  std::string Execute(std::string command);

  bool IsCommandSupported(std::string command);

 private:
  typedef std::string Command;
  const Options &options_;

  CommandExecutorObject(const Options &options);
  std::string GetServerProcessPid() const;
  std::string GetServerProcessPidfile() const;
};

}

}

#endif /* SRC_PROGRAM_OPTIONS_WEB_COMMAND_EXECUTOR_OBJECT_H */