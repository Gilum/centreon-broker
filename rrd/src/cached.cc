/*
** Copyright 2011 Merethis
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <assert.h>
#include <QFile>
#include <QLocalSocket>
#include <QTcpSocket>
#include <sstream>
#include <stdlib.h>
#include "exceptions/basic.hh"
#include "rrd/cached.hh"
#include "rrd/lib.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::rrd;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  Any attempt to use the copy constructor will result in a call to
 *  abort().
 *
 *  @param[in] c Unused.
 */
cached::cached(cached const& c) : backend(c) {
  assert(false);
  abort();
}

/**
 *  Assignment operator.
 *
 *  @param[in] c Unused.
 *
 *  @return This object.
 */
cached& cached::operator=(cached const& c) {
  (void)c;
  assert(false);
  abort();
  return (*this);
}

/**
 *  Send data to rrdcached.
 *
 *  @param[in] command Command to send.
 *  @param[in] size    Size of command. If 0, set to strlen(command).
 */
void cached::_send_to_cached(char const* command,
                             unsigned int size) {
  // Check socket.
  if (!_socket.data())
    throw (exceptions::basic() << "attempt to communicate with rrdcached without connecting first");

  // Check command size.
  if (!size)
    size = strlen(command);

  // Write data.
  while (size > 0) {
    qint64 rb;
    rb = _socket->write(command, size);
    if (rb < 0)
      throw (exceptions::basic() << "error while sending command to rrdcached: "
               << _socket->errorString().toStdString().c_str());
    size -= rb;
  }

  // Read response.
  if (!_batch) {
    char line[1024];
    if (_socket->readLine(line, sizeof(line)) < 0)
      throw (exceptions::basic() << "error while getting response from rrdcached: "
               << _socket->errorString().toStdString().c_str());
    unsigned int lines;
    lines = strtoul(line, NULL, 10);
    while (lines > 0)
      if (_socket->readLine(line, sizeof(line)) < 0)
        throw (exceptions::basic() << "error while getting response from rrdcached: "
                 << _socket->errorString().toStdString().c_str());
  }

  return ;
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
cached::cached() : _batch(false) {}

/**
 *  Destructor.
 */
cached::~cached() {}

/**
 *  Initiates the bulk load of multiple commands.
 */
void cached::begin() {
  // Send BATCH command to rrdcached.
  char buffer[] = "BATCH\n";
  _send_to_cached(buffer, sizeof(buffer) - 1);
  _batch = true;
  return ;
}

/**
 *  Close the current RRD file.
 */
void cached::close() {
  _filename.clear();
  _metric.clear();
  _batch = false;
  return ;
}

/**
 *  Commit current transaction.
 */
void cached::commit() {
  if (_batch) {
    // Send a . on the line to indicate that transaction is over.
    _batch = false;
    char buffer[] = ".\n";
    _send_to_cached(buffer, sizeof(buffer) - 1);
  }
  return ;
}

/**
 *  Connect to a local socket.
 *
 *  @param[in] name Socket name.
 */
void cached::connect_local(QString const& name) {
  // Create socket object.
  QLocalSocket* ls(new QLocalSocket);
  _socket.reset(ls);

  // Connect to server.
  ls->connectToServer(name);
  if (!ls->waitForConnected(-1)) {
    exceptions::basic e;
    e << "could not connect to local socket '" << name.toStdString().c_str()
      << ": " << ls->errorString().toStdString().c_str();
    _socket.reset();
    throw (e);
  }

  return ;
}

/**
 *  Connect to a remote server.
 *
 *  @param[in] address Server address.
 *  @param[in] port    Port to connect to.
 */
void cached::connect_remote(QString const& address,
                            unsigned short port) {
  // Create socket object.
  QTcpSocket* ts(new QTcpSocket);
  _socket.reset(ts);

  // Connect to server.
  ts->connectToHost(address, port);
  if (!ts->waitForConnected(-1)) {
    exceptions::basic e;
    e << "could not connect to remote server '" << address.toStdString().c_str()
      << ":" << port << "': "
      << ts->errorString().toStdString().c_str();
    _socket.reset();
    throw (e);
  }

  return ;
}

/**
 *  Open a RRD file which already exists.
 *
 *  @param[in] filename Path to the RRD file.
 *  @param[in] metric   Metric name.
 */
void cached::open(QString const& filename,
                  QString const& metric) {
  // Close previous file.
  this->close();

  // Check that the file exists.
  if (!QFile::exists(filename))
    throw (exceptions::basic() << "RRD file '" << filename.toStdString().c_str() << "' does not exist");

  // Remember information for further operations.
  _filename = filename;
  _metric = metric;

  return ;
}

/**
 *  Open a RRD file and create it if it does not exists.
 *
 *  @param[in] filename Path to the RRD file.
 *  @param[in] metric   Metric name.
 *  @param[in] length   Number of recording in the RRD file.
 *  @param[in] from     Timestamp of the first record.
 *  @param[in] interval Time interval between each record.
 */
void cached::open(QString const& filename,
                  QString const& metric,
                  unsigned int length,
                  time_t from,
                  time_t interval) {
  // Close previous file.
  this->close();

  // Remember informations for further operations.
  _filename = filename;
  _metric = metric;

  /* We are unfortunately forced to use librrd to create RRD file as
  ** rrdcached does not support RRD file creation.
  */
  lib rrdf;
  rrdf.open(filename, metric, length, from, interval);

  return ;
}

/**
 *  Update the RRD file with new value.
 *
 *  @param[in] t     Timestamp of value.
 *  @param[in] value Associated value.
 */
void cached::update(time_t t, QString const& value) {
  // Build rrdcached command.
  std::ostringstream oss;
  oss << "UPDATE " << _filename.toStdString() << " " << t
      << ":" << value.toStdString();

  // Send command.
  _send_to_cached(oss.str().c_str());

  return ;
}