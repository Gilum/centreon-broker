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

#ifndef CCB_DUMPER_ENTRIES_BA_HH
#  define CCB_DUMPER_ENTRIES_BA_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                 dumper {
  namespace               entries {
    /**
     *  @class ba ba.hh "com/centreon/broker/dumper/entries/ba.hh"
     *  @brief BA entry.
     *
     *  Some BA row of the BA configuration table.
     */
    class                 ba : public io::data {
    public:
                          ba();
                          ba(ba const& other);
                          ~ba();
      ba&                 operator=(ba const& other);
      bool                operator==(ba const& other) const;
      bool                operator!=(ba const& other) const;
      unsigned int        type() const;
      static unsigned int static_type();

      bool                enable;
      unsigned int        poller_id;
      unsigned int        ba_id;
      QString             description;
      double              level_critical;
      double              level_warning;
      QString             name;
      unsigned int        organization_id;
      unsigned int        type_id;

      static mapping::entry const
                          entries[];
      static io::event_info::event_operations const
                          operations;

    private:
      void                _internal_copy(ba const& other);
    };
  }
}

CCB_END()

#endif // !CCB_DUMPER_ENTRIES_BA_HH
