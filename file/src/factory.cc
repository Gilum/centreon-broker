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

#include <memory>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/file/factory.hh"
#include "com/centreon/broker/file/opener.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
factory::factory() {}

/**
 *  Copy constructor.
 *
 *  @param[in] f Object to copy.
 */
factory::factory(factory const& f) : io::factory(f) {}

/**
 *  Destructor.
 */
factory::~factory() {}

/**
 *  Assignment operator.
 *
 *  @param[in] f Object to copy.
 *
 *  @return This object.
 */
factory& factory::operator=(factory const& f) {
  io::factory::operator=(f);
  return (*this);
}

/**
 *  Clone this object.
 *
 *  @return Exact copy of this object.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  Check if a configuration match the file layer.
 *
 *  @param[in] cfg       Endpoint configuration.
 *  @param[in] is_input  true if the file should act as input.
 *  @param[in] is_output true if the file should act as output.
 *
 *  @return true if configuration matches the file layer.
 */
bool factory::has_endpoint(
                config::endpoint& cfg,
                bool is_input,
                bool is_output) const {
  (void)is_input;
  (void)is_output;
  bool retval;
  if (cfg.type == "file") {
    cfg.params["coarse"] = "yes"; // File won't respond to any salutation.
    retval = true;
  }
  else
    retval = false;
  return (retval);
}

/**
 *  Generate an endpoint matching a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[in]  is_input    true if the file should act as input.
 *  @param[in]  is_output   true if the file should act as output.
 *  @param[out] is_acceptor Will be set to false.
 *
 *  @return Acceptor matching configuration.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool is_input,
                         bool is_output,
                         bool& is_acceptor) const {
  (void)is_acceptor;

  // Find path to the file.
  QString filename;
  {
    QMap<QString, QString>::const_iterator it(cfg.params.find("path"));
    if (it == cfg.params.end())
      throw (exceptions::msg() << "file: no 'path' defined for file "
             "endpoint '" << cfg.name << "'");
    filename = *it;
  }

  // Find max size of file.
  unsigned long long max_size;
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("max_size"));
    if (it != cfg.params.end())
      max_size = it->toULongLong();
    else
      max_size = 0;
  }

  // Generate opener.
  std::auto_ptr<opener> openr(new opener(is_input, is_output));
  openr->set_filename(filename);
  openr->set_max_size(max_size);
  return (openr.release());
}
