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

#include "com/centreon/broker/correlation/node.hh"

using namespace com::centreon::broker;

/**
 *  Check that link creation works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Objects.
  correlation::node n1;
  correlation::node n2;

  // Create link.
  n1.add_depended(&n2);

  // Check.
  return (n1.depended_by().isEmpty()
          || (*n1.depended_by().begin() != &n2)
          || n2.depends_on().isEmpty()
          || (*n2.depends_on().begin() != &n1));
}
