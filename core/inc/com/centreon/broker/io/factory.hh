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

#ifndef CCB_IO_FACTORY_HH
#  define CCB_IO_FACTORY_HH

#  include "com/centreon/broker/config/endpoint.hh"
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace             io {
  /**
   *  @class factory factory.hh "com/centreon/broker/io/factory.hh"
   *  @brief Endpoint factory.
   *
   *  Build endpoint according to some configuration.
   */
  class               factory {
  public:
                      factory();
                      factory(factory const& f);
    virtual           ~factory();
    factory&          operator=(factory const& f);
    virtual factory*  clone() const = 0;
    virtual bool      has_endpoint(
                        com::centreon::broker::config::endpoint& cfg,
                        bool is_input,
                        bool is_output) const = 0;
    virtual bool      has_not_endpoint(
                        com::centreon::broker::config::endpoint& cfg,
                        bool is_input,
                        bool is_output) const;
    virtual endpoint* new_endpoint(
                        com::centreon::broker::config::endpoint& cfg,
                        bool is_input,
                        bool is_output,
                        bool& is_acceptor) const = 0;
    virtual misc::shared_ptr<stream>
                      new_stream(
                        misc::shared_ptr<stream> to,
                        bool is_acceptor,
                        QString const& proto_name);
  };
}

CCB_END()

#endif // !CCB_IO_FACTORY_HH
