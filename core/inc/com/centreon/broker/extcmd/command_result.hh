/*
** Copyright 2015 Centreon
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

#ifndef CCB_EXTCMD_COMMAND_RESULT_HH
#  define CCB_EXTCMD_COMMAND_RESULT_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                extcmd {
  /**
   *  @class command_result command_result.hh "com/centreon/broker/extcmd/command_result.hh"
   *  @brief Represent an external command result.
   *
   *  This event is sent when an external command was executed, either
   *  successfully or not.
   */
  class                  command_result : public io::data {
  public:
                         command_result();
                         command_result(command_result const& other);
                         ~command_result();
    command_result&      operator=(command_result const& other);
    unsigned int         type() const;
    static unsigned int  static_type();

    int                  code;
    unsigned int         id;
    QString              msg;

  private:
    void                 _internal_copy(command_result const& other);
  };
}

CCB_END()

#endif // !CCB_EXTCMD_COMMAND_RESULT_HH
