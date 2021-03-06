#pragma once

#include <slas/dbus/detail/bus_interface.h>
#include <slas/network/detail/network_interface.h>
#include <slas/type/timestamp.h>

#include "src/dbus/detail/dbus_thread_interface.h"

#include <string>
#include <map>

namespace apache
{

class ApacheLogReceiver;
typedef std::shared_ptr<ApacheLogReceiver> ApacheLogReceiverPtr;

class ApacheLogReceiver {
 public:
  static ApacheLogReceiverPtr Create(dbus::detail::BusInterfacePtr bus,
                                     dbus::detail::DBusThreadInterfacePtr dbus_thread);

  virtual ~ApacheLogReceiver();

  void OpenSocket(const std::string &socket_path);
  void CloseSocket();

  void StartLoop();
  void StopLoop();

  bool IsRunning() const;

  void SetAgentName(const std::string &agent_name);

 private:
  ApacheLogReceiver(dbus::detail::BusInterfacePtr bus,
                    dbus::detail::DBusThreadInterfacePtr dbus_thread,
                    network::detail::NetworkInterfacePtr network);

  type::Timestamp LogTimestampToTimestamp(std::string timestamp);

  dbus::detail::BusInterfacePtr bus_;
  dbus::detail::DBusThreadInterfacePtr dbus_thread_;
  network::detail::NetworkInterfacePtr network_;

  int socket_fd_;
  bool running_;
  std::string logbuf;
  const std::map<std::string, int> months;
  std::string agent_name_;
};

}
