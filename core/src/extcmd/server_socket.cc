/*
** Copyright 2015 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/extcmd/server_socket.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::extcmd;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] path  Path on which server should listen.
 */
server_socket::server_socket(std::string const& path) {
  if (!listen(path.c_str()))
    throw (exceptions::msg() << "cannot listen on socket '"
           << path << "': " << error_string());
}

/**
 *  Destructor.
 */
server_socket::~server_socket() {}

/**
 *  Close socket.
 */
void server_socket::close() {
  QLocalServer::close();
  return ;
}

/**
 *  Get last error as a human-readable string.
 *
 *  @return Error string.
 */
QString server_socket::error_string() const {
  return (errorString());
}

/**
 *  Check if server has pending connections.
 *
 *  @return True if server has pending connections.
 */
bool server_socket::has_pending_connections() const {
  return (!_pending.empty());
}

/**
 *  Get next pending connection.
 *
 *  @return Next pending connection.
 */
int server_socket::next_pending_connection() {
  int next;
  if (!_pending.empty()) {
    next = _pending.front();
    _pending.pop();
  }
  else
    next = -1;
  return (next);
}

/**
 *  Wait for incoming connection for some time.
 *
 *  @param[in]  timeout_ms  Timeout in milliseconds.
 *  @param[out] timed_out   Set to true if connection timed out.
 *
 *  @return True if a connection is available.
 */
bool server_socket::wait_for_new_connection(
                      int timeout_ms,
                      bool* timed_out) {
  waitForNewConnection(timeout_ms, timed_out);
  return (has_pending_connections());
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Reimplemented method.
 *
 *  This method is called when a new connection arrives. It is store
 *  socket descriptors in a queue, processed by
 *  next_pending_connection(). Descriptors are used in a lazy fashion,
 *  allowing QLocalSocket objects to belong to their proper thread.
 *
 *  @param[in] socket_descriptor  Native socket descriptor.
*/
void server_socket::incomingConnection(quintptr socket_descriptor) {
  _pending.push(socket_descriptor);
  return ;
}
