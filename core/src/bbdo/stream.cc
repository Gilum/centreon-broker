/*
** Copyright 2013,2015 Merethis
**
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

#include <QStringList>
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/namespace.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
stream::stream()
  : _coarse(false),
    _negociate(true),
    _negociated(false),
    _timeout(3) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
stream::stream(stream const& other)
  : io::stream(other),
    input(other),
    output(other),
    _coarse(other._coarse),
    _extensions(other._extensions),
    _negociate(other._negociate),
    _negociated(other._negociated),
    _timeout(other._timeout) {}

/**
 *  Destructor.
 */
stream::~stream() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& other) {
  if (this != &other) {
    input::operator=(other);
    output::operator=(other);
    _coarse = other._coarse;
    _extensions = other._extensions;
    _negociate = other._negociate;
    _negociated = other._negociated;
    _timeout = other._timeout;
  }
  return (*this);
}

/**
 *  Negociate features with peer.
 *
 *  @param[in] neg  Negociation type.
 */
void stream::negociate(stream::negociation_type neg) {
  // Coarse peer don't expect any salutation either.
  if (_coarse) {
    _negociated = true;
    return ;
  }
  else if (_negociated)
    return ;

  // Send our own packet if we should be first.
  if (neg == negociate_first) {
    logging::debug(logging::medium)
      << "BBDO: sending welcome packet (available extensions: "
      << (_negociate ? _extensions : "") << ")";
    misc::shared_ptr<version_response>
      welcome_packet(new version_response);
    if (_negociate)
      welcome_packet->extensions = _extensions;
    output::write(welcome_packet);
    output::write(misc::shared_ptr<io::data>());
  }

  // Read peer packet.
  logging::debug(logging::medium)
    << "BBDO: retrieving welcome packet of peer";
  misc::shared_ptr<io::data> d;
  read_any(d, _timeout);
  if (d.isNull() || (d->type() != version_response::static_type()))
    throw (exceptions::msg()
           << "BBDO: invalid protocol header, aborting connection");

  // Handle protocol version.
  misc::shared_ptr<version_response>
    v(d.staticCast<version_response>());
  if (v->bbdo_major != BBDO_VERSION_MAJOR)
    throw (exceptions::msg()
           << "BBDO: peer is using protocol version " << v->bbdo_major
           << "." << v->bbdo_minor << "." << v->bbdo_patch
           << " whereas we're using protocol version "
           << BBDO_VERSION_MAJOR << "." << BBDO_VERSION_MINOR << "."
           << BBDO_VERSION_PATCH);
  logging::info(logging::medium)
    << "BBDO: peer is using protocol version " << v->bbdo_major
    << "." << v->bbdo_minor << "." << v->bbdo_patch
    << ", we're using version " << BBDO_VERSION_MAJOR << "."
    << BBDO_VERSION_MINOR << "." << BBDO_VERSION_PATCH;

  // Send our own packet if we should be second.
  if (neg == negociate_second) {
    logging::debug(logging::medium)
      << "BBDO: sending welcome packet (available extensions: "
      << (_negociate ? _extensions : "") << ")";
    misc::shared_ptr<version_response>
      welcome_packet(new version_response);
    if (_negociate)
      welcome_packet->extensions = _extensions;
    output::write(welcome_packet);
    output::write(misc::shared_ptr<io::data>());
  }

  // Negociation.
  if (_negociate) {
    // Apply negociated extensions.
    logging::info(logging::medium)
      << "BBDO: we have extensions '"
      << _extensions << "' and peer has '" << v->extensions << "'";
    QStringList own_ext(_extensions.split(' '));
    QStringList peer_ext(v->extensions.split(' '));
    for (QStringList::const_iterator
           it(own_ext.begin()),
           end(own_ext.end());
         it != end;
         ++it) {
      // Find matching extension in peer extension list.
      QStringList::const_iterator
        peer_it(std::find(peer_ext.begin(), peer_ext.end(), *it));
      // Apply extension if found.
      if (peer_it != peer_ext.end()) {
        logging::info(logging::medium)
          << "BBDO: applying extension '" << *it << "'";
        for (QMap<QString, io::protocols::protocol>::const_iterator
               proto_it(io::protocols::instance().begin()),
               proto_end(io::protocols::instance().end());
             proto_it != proto_end;
             ++proto_it)
          if (proto_it.key() == *it) {
            misc::shared_ptr<io::stream>
              s(proto_it->endpntfactry->new_stream(
                                          _substream,
                                          neg == negociate_second,
                                          *it));
            set_substream(s);
            break ;
          }
      }
    }
  }

  // Stream has now negociated.
  _negociated = true;
  return ;
}

/**
 *  Read data from stream.
 *
 *  @param[out] d         Next available event.
 *  @param[in]  deadline  Deadline.
 *
 *  @return Respect io::stream::read() return value.
 *
 *  @see input::read()
 */
bool stream::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  if (!_negociated)
    negociate(negociate_second);
  return (input::read(d, deadline));
}

/**
 *  Set whether this stream is coarse or not.
 *
 *  @param[in] coarse  True if coarse.
 */
void stream::set_coarse(bool coarse) {
  _coarse = coarse;
  return ;
}

/**
 *  Set whether or not the stream should negociate features.
 *
 *  @param[in] negociate   True if the stream should negociate features.
 *  @param[in] extensions  Extensions supported by this stream.
 */
void stream::set_negociate(bool negociate, QString const& extensions) {
  _negociate = negociate;
  _extensions = extensions;
  return ;
}

/**
 *  Set the timeout supported by this stream.
 *
 *  @param[in] timeout  Timeout in seconds.
 */
void stream::set_timeout(int timeout) {
  _timeout = timeout;
  return ;
}

/**
 *  Get statistics.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(io::properties& tree) const {
  output::statistics(tree);
  return ;
}

/**
 *  Write data to stream.
 *
 *  @param[in] d Data to send.
 *
 *  @return Number of events acknowledged.
 */
unsigned int stream::write(misc::shared_ptr<io::data> const& d) {
  if (!_negociated)
    negociate(negociate_second);
  return (output::write(d));
}