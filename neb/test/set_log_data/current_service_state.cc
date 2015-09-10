/*
** Copyright 2011 Centreon
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

#include "com/centreon/broker/neb/log_entry.hh"
#include "com/centreon/broker/neb/set_log_data.hh"

using namespace com::centreon::broker;

/**
 *  Check that a current service state log is properly parsed.
 *
 *  @return 0 on success.
 */
int main() {
  // Log entry.
  neb::log_entry le;

  // Parse a current service state line.
  neb::set_log_data(le,
    "CURRENT SERVICE STATE: myserver;myservice;UNKNOWN;HARD;2;Requested table is empty.");

  // Check that it was properly parsed.
  return ((le.host_name != "myserver")
          || (le.log_type != 1) // HARD
          || (le.msg_type != 6) // CURRENT_SERVICE_STATE
          || (le.output != "Requested table is empty.")
          || (le.retry != 2)
          || (le.status != 3)); // UNKNOWN
}
