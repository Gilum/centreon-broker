/*
** Copyright 2011-2013 Centreon
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

#include <QCoreApplication>
#include <QObject>
#include <QThread>
#include <QTimer>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/exceptions/with_pointer.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/processing/feeder.hh"
#include "test/processing/feeder/common.hh"
#include "test/processing/feeder/setable_endpoint.hh"
#include "test/processing/feeder/setable_stream.hh"

using namespace com::centreon::broker;

/**
 *  Run the feeder thread and check that it works properly.
 */
class wrapper : public QThread {
private:
  misc::shared_ptr<setable_stream>
       _in;
  misc::shared_ptr<setable_stream>
       _out;
  bool _triggered;

public:
  /**
   *  Default constructor.
   */
       wrapper(
         misc::shared_ptr<setable_stream> in,
         misc::shared_ptr<setable_stream> out)
    : _in(in), _out(out), _triggered(false) {}

  /**
   *  Destructor.
   */
       ~wrapper() {}

  /**
   *  Check if exception was triggered.
   *
   *  @return true if proper exception was triggered.
   */
  bool triggered() const {
    return (_triggered);
  }

  /**
   *  Thread entry point (check method).
   */
  void run() {
    processing::feeder f;
    f.prepare(
      _in.staticCast<io::stream>(),
      _out.staticCast<io::stream>());
    try {
      f.run();
    }
    catch (exceptions::with_pointer const& e) {
      _out->process(true, true);
      _out->write(e.ptr());
      _triggered = true;
    }
    return ;
  }
};

/**
 *  Check that simple event feeding works properly.
 *
 *  @param[in] argc Arguments count.
 *  @param[in] argv Arguments values.
 *
 *  @return 0 on success.
 */
int main(int argc, char* argv[]) {
  // Qt core application.
  QCoreApplication app(argc, argv);

  // Initialization.
  config::applier::init();

  // Enable logging.
  if (argc > 1)
    log_on_stderr();

  // Streams.
  misc::shared_ptr<setable_stream> ss1(new setable_stream);
  misc::shared_ptr<setable_stream> ss2(new setable_stream);
  ss1->process(true, true);
  ss2->process(true, true);
  ss2->set_store_events(true);

  // Wrapping object.
  wrapper w(ss1, ss2);

  // Launch check thread.
  QObject::connect(&w, SIGNAL(finished()), &app, SLOT(quit()));
  QObject::connect(&w, SIGNAL(started()), &app, SLOT(quit()));
  QObject::connect(&w, SIGNAL(terminated()), &app, SLOT(quit()));
  w.start();

  // Wait some time.
  app.exec();
  QTimer::singleShot(200, &app, SLOT(quit()));
  app.exec();

  // Set failing flag.
  ss2->process(false, false);

  // Wait for thread termination.
  w.wait();

  // Check output content.
  int retval(0);
  unsigned int count(ss1->get_count());
  unsigned int i(0);
  for (QList<misc::shared_ptr<io::data> >::const_iterator
         it(ss2->get_stored_events().begin()),
         end(ss2->get_stored_events().end());
       it != end;
       ++it)
    if ((*it)->type() != io::events::data_type<io::events::internal, 1>::value)
      retval |= 1;
    else {
      misc::shared_ptr<io::raw> raw(it->staticCast<io::raw>());
      unsigned int val;
      memcpy(&val, raw->QByteArray::data(), sizeof(val));
      retval |= (val != ++i);
    }
  retval |= (i != count);
  retval |= !w.triggered();

  // Cleanup.
  config::applier::deinit();

  // Return check result.
  return (retval);
}
