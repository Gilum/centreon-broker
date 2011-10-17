/*
** Copyright 2011 Merethis
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

#include <string.h>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"

using namespace com::centreon::broker;

#define MSG1 "0123456789abcdef"
#define MSG2 "foo bar baz qux"

/**
 *  We should not be able to read from publisher.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  config::applier::init();

  // Publisher.
  multiplexing::publisher p;

  // Subscriber.
  multiplexing::subscriber s;

  // Publish event.
  {
    QSharedPointer<io::raw> raw(new io::raw);
    raw->append(MSG1);
    p.write(raw.staticCast<io::data>());
  }

  // Launch multiplexing.
  multiplexing::engine::instance().start();

  // Publish another event.
  {
    QSharedPointer<io::raw> raw(new io::raw);
    raw->append(MSG2);
    p.write(raw.staticCast<io::data>());
  }

  // Check data.
  int retval(0);
  char const* messages[] = { MSG1, MSG2, NULL };
  for (unsigned int i = 0; messages[i]; ++i) {
    QSharedPointer<io::data> data(s.read(0));
    if (data.isNull()
        || (data->type() != "com::centreon::broker::io::raw"))
      retval |= 1;
    else {
      QSharedPointer<io::raw> raw(data.staticCast<io::raw>());
      retval |= strncmp(
        raw->QByteArray::data(),
        messages[i],
        strlen(messages[i]));
    }
  }

  // Return.
  return (retval);
}