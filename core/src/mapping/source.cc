/*
** Copyright 2011,2015 Centreon
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

#include "com/centreon/broker/mapping/source.hh"

using namespace com::centreon::broker::mapping;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
source::source() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
source::source(source const& other) {
  (void)other;
}

/**
 *  Destructor.
 */
source::~source() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
source& source::operator=(source const& other) {
  (void)other;
  return (*this);
}
