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

#ifndef CCB_STORAGE_METRIC_HH
#  define CCB_STORAGE_METRIC_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/io/events.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/storage/internal.hh"

CCB_BEGIN()

namespace          storage {
  /**
   *  @class metric metric.hh "com/centreon/broker/storage/metric.hh"
   *  @brief Metric information.
   *
   *  Metric information, mainly used to update RRD files.
   */
  class            metric : public io::data {
  public:
                   metric();
                   metric(metric const& m);
                   ~metric();
    metric&        operator=(metric const& m);
    unsigned int   type() const;

    /**
     *  Get the type of this event.
     *
     *  @return  The event type.
     */
    static unsigned int
                   static_type() {
      return (io::events::data_type<
                            io::events::storage,
                            storage::de_metric>::value);
    }


    timestamp      ctime;
    unsigned int   interval;
    bool           is_for_rebuild;
    unsigned int   metric_id;
    QString        name;
    int            rrd_len;
    double         value;
    short          value_type;
    unsigned int   host_id;
    unsigned int   service_id;

    static mapping::entry const
                    entries[];
    static io::event_info::event_operations const
                    operations;

  private:
    void           _internal_copy(metric const& m);
  };
}

CCB_END()

#endif // !CCB_STORAGE_METRIC_HH
