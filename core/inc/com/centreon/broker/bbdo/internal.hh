/*
** Copyright 2013 Centreon
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

#ifndef CCB_BBDO_INTERNAL_HH
#  define CCB_BBDO_INTERNAL_HH

#  include "com/centreon/broker/namespace.hh"

#  define BBDO_HEADER_SIZE 16
#  define BBDO_VERSION_MAJOR 2
#  define BBDO_VERSION_MINOR 0
#  define BBDO_VERSION_PATCH 0
#  define BBDO_INTERNAL_TYPE 65535u

CCB_BEGIN()

namespace                 bbdo {
  // Data elements.
  enum {
    de_version_response = 1,
    de_ack
  };

  // Load/unload of BBDO protocol.
  void                    load();
  void                    unload();
}

CCB_END()

#endif // !CCB_BBDO_INTERNAL_HH
