/*
** Copyright 2009-2013 Centreon
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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/host_group_member.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
host_group_member::host_group_member() {}

/**
 *  Copy constructor.
 *
 *  @param[in] hgm Object to copy.
 */
host_group_member::host_group_member(host_group_member const& hgm)
  : group_member(hgm) {}

/**
 *  Destructor.
 */
host_group_member::~host_group_member() {}

/**
 *  Assignement operator.
 *
 *  @param[in] hgm Object to copy.
 *
 *  @return This object.
 */
host_group_member& host_group_member::operator=(host_group_member const& hgm) {
  group_member::operator=(hgm);
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int host_group_member::type() const {
  return (io::events::data_type<io::events::neb, neb::de_host_group_member>::value);
}
