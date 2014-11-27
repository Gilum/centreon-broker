/*
** Copyright 2011-2014 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <sstream>
#include <iomanip>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/notification/utilities/qhash_func.hh"
#include "com/centreon/broker/notification/macro_generator.hh"
#include "com/centreon/broker/notification/utilities/get_datetime_string.hh"
#include "com/centreon/broker/notification/macro_getters.hh"

using namespace com::centreon::broker::notification;

/**
 *  Default constructor.
 */
macro_generator::macro_generator() {
  if (_map.empty())
    _fill_x_macro_map(_map);
}

/**
 *  Generate macros.
 *
 *  @param[in,out] container  A container of asked macro, which will be filled as a result.
 *  @param[in] id             The id of the node for which we create macros.
 *  @param[in] cnt            The contact.
 *  @param[in] st             The state.
 *  @param[in] cache          The node cache.
 */
void macro_generator::generate(
                        macro_container& container,
                        objects::node_id id,
                        objects::contact const& cnt,
                        state const& st,
                        node_cache const& cache) const {
  objects::node::ptr node = st.get_node_by_id(id);
  if (!node)
    throw (exceptions::msg()
           << "notification: macro_generator: can't find the node: "
           << id.get_host_id() << ", " << id.get_service_id());
  objects::node::ptr host = node;
  if (id.is_service())
    host = st.get_node_by_id(objects::node_id(id.get_host_id()));
  if (!host)
    throw (exceptions::msg()
           << "notification: macro_generator: can't find the host "
           << id.get_host_id());
  node_cache::host_node_state const& hns = cache.get_host(id.get_host_id());

  for (macro_container::iterator it(container.begin()),
                                 end(container.end());
       it != end;
       ++it) {
    if (_get_global_macros(it.key(), st, *it))
      continue ;
    else if (_get_x_macros(it.key(), id, st, cache, cnt, *it))
      continue ;
  }
}

/**
 *  Get this macro if it's a global macro.
 *
 *  @param[in] macro_name  The name of the macro.
 *  @param[in] st          The state.
 *  @param[out] result     The result, filled if the macro is global.
 *  @return                True if this macro was found in the global macros.
 */
bool macro_generator::_get_global_macros(
                        std::string const& macro_name,
                        state const& st,
                        std::string& result) {
  QHash<std::string, std::string> const& global_macros =
                                           st.get_global_macros();
  QHash<std::string, std::string>::const_iterator found =
    global_macros.find(macro_name);
  if (found == global_macros.end())
    return (false);
  result = *found;
  return (true);
}

bool macro_generator::_get_x_macros(
                        std::string const& macro_name,
                        objects::node_id id,
                        state const& st,
                        node_cache const& cache,
                        objects::contact const& cnt,
                        std::string& result) const {
  x_macro_map::const_iterator found = _map.find(macro_name);
  if (found == _map.end())
    return (false);
  else {
    result = (*found)(id, st, cache, cnt);
    return (true);
  }
  get_contact_member<std::string const&, &objects::contact::get_name, 0>(id, st, cache, cnt);
}

/**
 *  Fill the macro generator map with functions used to generate macros.
 *
 *  @param[in] map  The map to fill.
 */
void macro_generator::_fill_x_macro_map(x_macro_map& map) {
  // Time macros.
  map.insert(
    "LONGDATETIME",
    &get_datetime_string<utilities::long_date_time>);
  map.insert(
    "SHORTDATETIME",
    &get_datetime_string<utilities::short_date_time>);
  map.insert(
    "DATE",
    &get_datetime_string<utilities::short_date>);
  map.insert(
    "TIME",
    &get_datetime_string<utilities::short_time>);
  map.insert(
    "TIMET",
    &get_timet_string);

  // Host specific macros.
  map.insert(
    "HOSTDISPLAYNAME",
    &get_host_member_as_string<
      neb::host_service,
      QString,
      &neb::host_service::display_name,
      0>);
  map.insert(
    "HOSTALIAS",
    &get_host_member_as_string<neb::host, QString, &neb::host::alias, 0>);
  map.insert(
    "HOSTADDRESS",
    &get_host_member_as_string<neb::host, QString, &neb::host::address, 0>);
  map.insert(
    "HOSTSTATE",
    &get_host_state);
  map.insert(
    "HOSTSTATEID",
    &get_host_status_member_as_string<
      neb::host_service_status,
      short,
      &neb::host_service_status::current_state,
      0>);
  map.insert(
    "LASTHOSTSTATE",
    &get_last_host_state);
  map.insert(
    "LASTHOSTSTATEID",
    &get_host_prevstatus_member_as_string<
      neb::host_service_status,
      short,
      &neb::host_service_status::current_state,
      0>);
  map.insert(
    "HOSTSTATETYPE",
    &get_host_state_type);
  map.insert(
    "HOSTATTEMPT",
    &get_host_status_member_as_string<
      neb::host_service_status,
      short,
      &neb::host_service_status::current_check_attempt,
      0>);
  map.insert(
    "MAXHOSTATTEMPTS",
    &get_host_status_member_as_string<
      neb::host_service_status,
      short,
      &neb::host_service_status::max_check_attempts,
      0>);
  // Event and problem id macros are ignored.
  map.insert("HOSTEVENTID", &null_getter);
  map.insert("LASTHOSTEVENTID", &null_getter);
  map.insert("HOSTPROBLEMID", &null_getter);
  map.insert("LASTHOSTPROBLEMID", &null_getter);
  map.insert(
    "HOSTLATENCY",
    &get_host_status_member_as_string<
      neb::host_service_status,
      double,
      &neb::host_service_status::latency,
      3>);
  map.insert(
    "HOSTEXECUTIONTIME",
    &get_host_status_member_as_string<
      neb::host_service_status,
      double,
      &neb::host_service_status::execution_time,
      3>);
  map.insert(
    "HOSTDURATION",
    get_host_duration);
  map.insert(
    "HOSTDURATIONSEC",
    get_host_duration_sec);
  map.insert(
    "HOSTDOWNTIME",
    &get_host_status_member_as_string<
      neb::host_service_status,
      short,
      &neb::host_service_status::scheduled_downtime_depth,
      0>);
  map.insert(
    "HOSTPERCENTCHANGE",
    &get_host_status_member_as_string<
      neb::host_service_status,
      double,
      &neb::host_service_status::percent_state_change,
      2>);
  map.insert(
    "HOSTGROUPNAME",
    &get_host_groups<false>);
  map.insert(
    "HOSTGROUPNAMES",
    &get_host_groups<true>);
  map.insert(
    "LASTHOSTCHECK",
    &get_host_status_member_as_string<
      neb::host_service_status,
      timestamp,
      &neb::host_service_status::last_check,
      0>);
  map.insert(
    "LASTHOSTSTATECHANGE",
    &get_host_status_member_as_string<
      neb::host_service_status,
      timestamp,
      &neb::host_service_status::last_state_change,
      0>);
  map.insert(
    "LASTHOSTUP",
    &get_host_status_member_as_string<
      neb::host_status,
      timestamp,
      &neb::host_status::last_time_up,
      0>);
  map.insert(
    "LASTHOSTDOWN",
    &get_host_status_member_as_string<
      neb::host_status,
      timestamp,
      &neb::host_status::last_time_down,
      0>);
  map.insert(
    "LASTHOSTUNREACHABLE",
    &get_host_status_member_as_string<
      neb::host_status,
      timestamp,
      &neb::host_status::last_time_unreachable,
      0>);
  map.insert(
    "HOSTOUTPUT",
    &get_host_output<false>);
  map.insert(
    "LONGHOSTOUTPUT",
    &get_host_output<true>);
  map.insert(
    "HOSTPERFDATA",
    &get_host_status_member_as_string<
      neb::host_service_status,
      QString,
      &neb::host_service_status::perf_data,
      0>);
  map.insert(
    "HOSTCHECKCOMMAND",
    &get_host_status_member_as_string<
      neb::host_service_status,
      QString,
      &neb::host_service_status::check_command,
      0>);
  // Hst ack macros are deprecated and ignored.
  map.insert("HOSTACKAUTHOR", &null_getter);
  map.insert("HOSTACKAUTHORNAME", &null_getter);
  map.insert("HOSTACKAUTHORALIAS", &null_getter);
  map.insert("HOSTACKCOMMENT", &null_getter);
  map.insert(
    "TOTALHOSTSERVICES",
    &get_total_host_services<-1>);
  map.insert(
    "TOTALHOSTSERVICESOK",
    &get_total_host_services<objects::node_state::service_ok>);
  map.insert(
    "TOTALHOSTSERVICESWARNING",
    &get_total_host_services<objects::node_state::service_warning>);
  map.insert(
    "TOTALHOSTSERVICESUNKNOWN",
    &get_total_host_services<objects::node_state::service_unknown>);
  map.insert(
    "TOTALHOSTSERVICESCRITICAL",
    &get_total_host_services<objects::node_state::service_critical>);
  // Service macros
  map.insert(
    "SERVICEDESC",
    &get_service_status_member_as_string<
      neb::service_status,
      QString,
      &neb::service_status::service_description,
      0>);
  map.insert(
    "SERVICEDISPLAYNAME",
    &get_service_member_as_string<
      neb::host_service,
      QString,
      &neb::host_service::display_name,
      0>);
  map.insert(
    "SERVICESTATE",
    &get_service_state);
  map.insert(
    "SERVICESTATEID",
    &get_service_status_member_as_string<
      neb::host_service_status,
      short,
      &neb::host_service_status::current_state,
      0>);
  map.insert(
    "LASTSERVICESTATE",
    &get_last_service_state);
  map.insert(
    "LASTSERVICESTATEID",
    &get_service_prevstatus_member_as_string<
      neb::host_service_status,
      short,
      &neb::host_service_status::current_state,
      0>);
  map.insert(
    "SERVICESTATETYPE",
    &get_service_state_type);
  map.insert(
    "SERVICEATTEMPT",
    &get_service_status_member_as_string<
      neb::host_service_status,
      short,
      &neb::host_service_status::current_check_attempt,
      0>);
  map.insert(
    "MAXSERVICEATTEMPTS",
    &get_service_status_member_as_string<
      neb::host_service_status,
      short,
      &neb::host_service_status::max_check_attempts,
      0>);
  map.insert(
    "SERVICEISVOLATILE",
    &get_service_member_as_string<
      neb::service,
      bool,
      &neb::service::is_volatile,
      0>);
  // Event id are ignored.
  map.insert("SERVICEEVENTID", &null_getter);
  map.insert("LASTSERVICEEVENTID", &null_getter);
  map.insert("SERVICEPROBLEMID", &null_getter);
  map.insert("LASTSERVICEPROBLEMID", &null_getter);
  map.insert(
    "SERVICELATENCY",
    &get_service_status_member_as_string<
      neb::host_service_status,
      double,
      &neb::host_service_status::latency,
      3>);
  map.insert(
    "SERVICEEXECUTIONTIME",
    &get_service_status_member_as_string<
      neb::host_service_status,
      double,
      &neb::host_service_status::execution_time,
      3>);
  map.insert(
    "SERVICEDURATION",
    &get_service_duration);
  map.insert(
    "SERVICEDURATIONSEC",
    &get_service_duration_sec);
  map.insert(
    "SERVICEDOWNTIME",
    &get_service_status_member_as_string<
      neb::host_service_status,
      short,
      &neb::host_service_status::scheduled_downtime_depth,
      0>);
  map.insert(
    "SERVICEPERCENTCHANGE",
    &get_service_status_member_as_string<
      neb::host_service_status,
      double,
      &neb::host_service_status::percent_state_change,
      2>);
  map.insert(
    "SERVICEGROUPNAME",
    &get_service_groups<false>);
  map.insert(
    "SERVICEGROUPNAMES",
    &get_service_groups<true>);
  map.insert(
    "LASTSERVICECHECK",
    &get_service_status_member_as_string<
      neb::host_service_status,
      timestamp,
      &neb::host_service_status::last_check,
      0>);
  map.insert(
    "LASTSERVICESTATECHANGE",
    &get_service_status_member_as_string<
      neb::host_service_status,
      timestamp,
      &neb::host_service_status::last_state_change,
      0>);
  map.insert(
    "LASTSERVICEOK",
    &get_service_status_member_as_string<
      neb::service_status,
      timestamp,
      &neb::service_status::last_time_ok,
      0>);
  map.insert(
    "LASTSERVICEWARNING",
    &get_service_status_member_as_string<
      neb::service_status,
      timestamp,
      &neb::service_status::last_time_warning,
      0>);
  map.insert(
    "LASTSERVICEUNKNOWN",
    &get_service_status_member_as_string<
      neb::service_status,
      timestamp,
      &neb::service_status::last_time_unknown,
      0>);
  map.insert(
    "LASTSERVICECRITICAL",
    &get_service_status_member_as_string<
      neb::service_status,
      timestamp,
      &neb::service_status::last_time_critical,
      0>);
  map.insert(
    "SERVICEOUTPUT",
    &get_service_output<false>);
  map.insert(
    "LONGSERVICEOUTPUT",
    &get_service_output<true>);
  map.insert(
    "SERVICEPERFDATA",
    &get_service_status_member_as_string<
      neb::host_service_status,
      QString,
      &neb::host_service_status::perf_data,
      0>);
  map.insert(
    "SERVICECHECKCOMMAND",
    &get_service_status_member_as_string<
      neb::host_service_status,
      QString,
      &neb::host_service_status::check_command,
      0>);
  // Deprecated, ignored.
  map.insert("SERVICEACKAUTHOR", &null_getter);
  map.insert("SERVICEACKAUTHORNAME", &null_getter);
  map.insert("SERVICEACKAUTHORALIAS", &null_getter);
  map.insert("SERVICEACKCOMMENT", &null_getter);

  // Counting macros.
  map.insert(
    "TOTALHOSTSUP",
    &get_total_hosts<objects::node_state::host_up>);
  map.insert(
    "TOTALHOSTSDOWN",
    &get_total_hosts<objects::node_state::host_down>);
  map.insert(
    "TOTALHOSTSUNREACHABLE",
    &get_total_hosts<objects::node_state::host_unreachable>);
  map.insert(
    "TOTALHOSTSDOWNUNHANDLED",
    &get_total_hosts_unhandled<objects::node_state::host_down>);
  map.insert(
    "TOTALHOSTSUNREACHABLEUNHANDLED",
    &get_total_hosts_unhandled<objects::node_state::host_unreachable>);
  map.insert(
    "TOTALHOSTPROBLEMS",
    &get_total_hosts<-1>);
  map.insert(
    "TOTALHOSTPROBLEMSUNHANDLED",
    &get_total_hosts_unhandled<-1>);
  map.insert(
    "TOTALSERVICESOK",
    &get_total_services<objects::node_state::service_ok>);
  map.insert(
    "TOTALSERVICESWARNING",
    &get_total_services<objects::node_state::service_warning>);
  map.insert(
    "TOTALSERVICESCRITICAL",
    &get_total_services<objects::node_state::service_critical>);
  map.insert(
    "TOTALSERVICESUNKNOWN",
    &get_total_services<objects::node_state::service_unknown>);
  map.insert(
    "TOTALSERVICESWARNINGUNHANDLED",
    &get_total_services_unhandled<objects::node_state::service_warning>);
  map.insert(
    "TOTALSERVICESCRITICALUNHANDLED",
    &get_total_services_unhandled<objects::node_state::service_critical>);
  map.insert(
    "TOTALSERVICESUNKNOWNUNHANDLED",
    &get_total_services_unhandled<objects::node_state::service_unknown>);
  map.insert(
    "TOTALSERVICEPROBLEMS",
    &get_total_services<-1>);
  map.insert(
    "TOTALSERVICEPROBLEMSUNHANDLED",
    &get_total_services_unhandled<-1>);

  // Groups macros.
  map.insert(
    "HOSTGROUPALIAS",
    &get_group_alias<true>);
  map.insert(
    "HOSTGROUPMEMBERS",
    &get_group_members<true>);
  map.insert(
    "SERVICEGROUPALIAS",
    &get_group_alias<false>);
  map.insert(
    "SERVICEGROUPMEMBERS",
    &get_group_members<false>);

  // Contact macros.
  map.insert(
    "CONTACTNAME",
    &get_contact_member<std::string const&, &objects::contact::get_name, 0>);
  map.insert(
    "CONTACTALIAS",
    &get_contact_member<std::string const&, &objects::contact::get_alias, 0>);
  map.insert(
    "CONTACTEMAIL",
    &get_contact_member<std::string const&, &objects::contact::get_email, 0>);
  map.insert(
    "CONTACTPAGER",
    &get_contact_member<std::string const&, &objects::contact::get_pager, 0>);
  map.insert(
    "CONTACTADDRESS1",
    &get_address_of_contact<1>);
  map.insert(
    "CONTACTADDRESS2",
    &get_address_of_contact<2>);
  map.insert(
    "CONTACTADDRESS3",
    &get_address_of_contact<3>);
  map.insert(
    "CONTACTADDRESS4",
    &get_address_of_contact<4>);
  map.insert(
    "CONTACTADDRESS5",
    &get_address_of_contact<5>);
  map.insert(
    "CONTACTADDRESS6",
    &get_address_of_contact<6>);
  map.insert(
    "CONTACTGROUPALIAS",
    &get_contactgroup_alias);
  map.insert(
    "CONTACTGROUPMEMBERS",
    &get_contactgroup_members);
}
