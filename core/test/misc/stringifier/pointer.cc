/*
** Copyright 2011-2012 Centreon
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

#include <cstdlib>
#include "com/centreon/broker/misc/stringifier.hh"

using namespace com::centreon::broker;

/**
 *  Check that pointer insertion works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Insertion.
  misc::stringifier s;
  s << &s;

  // Check content.
  return ((&s != (void*)strtoull(s.data(), NULL, 0))
          ? EXIT_FAILURE
          : EXIT_SUCCESS);
}
