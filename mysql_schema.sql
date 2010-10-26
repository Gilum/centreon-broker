-- ------------------------------------
--                                   --
-- Centreon Broker's database schema --
--                                   --
-- ------------------------------------

-- acknowledgements
-- comments
-- customvariables
-- downtimes
-- eventhandlers
-- flappinghistory
-- hosts
-- host_dependency
-- host_parent
-- hostcommands
-- hostgroups
-- hosts_hostgroups
-- instances
-- issues
-- issue_parent
-- logs
-- notifications
-- schemaversion
-- services
-- service_dependency
-- service_servicegroup
-- servicecommands
-- servicegroup
-- states


--
-- Holds the current version of the database schema.
--
CREATE TABLE schemaversion (
  software varchar(128) NOT NULL,
  version int NOT NULL
);
INSERT INTO schemaversion (software, version) VALUES ('centreon-broker', 1);


--
-- Store information about Nagios instances.
--
CREATE TABLE instances (
  instance_id int NOT NULL,
  name varchar(255) NOT NULL default 'localhost',

  active_host_checks boolean default NULL,
  active_service_checks boolean default NULL,
  address varchar(128) default NULL,
  check_hosts_freshness boolean default NULL,
  check_services_freshness boolean default NULL,
  daemon_mode boolean default NULL,
  description varchar(128) default NULL,
  end_time int default NULL,
  event_handlers boolean default NULL,
  failure_prediction boolean default NULL,
  flap_detection boolean default NULL,
  global_host_event_handler text default NULL,
  global_service_event_handler text default NULL,
  last_alive int default NULL,
  last_command_check int default NULL,
  last_log_rotation int default NULL,
  modified_host_attributes int default NULL,
  modified_service_attributes int default NULL,
  notifications boolean default NULL,
  obsess_over_hosts boolean default NULL,
  obsess_over_services boolean default NULL,
  passive_host_checks boolean default NULL,
  passive_service_checks boolean default NULL,
  pid int default NULL,
  process_perfdata boolean default NULL,
  running boolean default NULL,
  start_time int default NULL,

  PRIMARY KEY (instance_id)
) ENGINE=InnoDB;


--
-- Holds acknowledgedments information.
--
CREATE TABLE acknowledgements (
  acknowledgement_id int NOT NULL auto_increment,
  entry_time int NOT NULL,
  host_name varchar(255) NOT NULL,
  instance_name varchar(255) NOT NULL,
  service_description varchar(255) default NULL,

  author varchar(64) default NULL,
  comment varchar(255) default NULL,
  notify_contacts boolean default NULL,
  persistent_comment boolean default NULL,
  state smallint default NULL,
  sticky boolean default NULL,
  type smallint default NULL,

  PRIMARY KEY (acknowledgement_id),
  UNIQUE (entry_time, host_name, instance_name, service_description)
) ENGINE=InnoDB;


--
-- Holds comments information.
--
CREATE TABLE comments (
  comment_id int NOT NULL auto_increment,
  entry_time int NOT NULL,
  instance_name varchar(255) NOT NULL,
  internal_id int NOT NULL,

  author varchar(64) default NULL,
  data text default NULL,
  deletion_time int default NULL,
  entry_type smallint default NULL,
  expire_time int default NULL,
  expires boolean default NULL,
  host_name varchar(255) NOT NULL,
  persistent boolean default NULL,
  service_description varchar(255) default NULL,
  source smallint default NULL,
  type smallint default NULL,

  PRIMARY KEY (id),
  UNIQUE (entry_time, instance_name, internal_id)
) ENGINE=InnoDB;


--
-- Monitored hosts.
--
CREATE TABLE hosts (
  host_id int NOT NULL,
  host_name varchar(255) NOT NULL,
  instance_id int NOT NULL,

  acknowledged boolean default NULL,
  acknowledgement_type smallint default NULL,
  action_url varchar(255) default NULL,
  active_checks boolean default NULL,
  address varchar(75) default NULL,
  alias varchar(100) default NULL,
  check_attempt smallint default NULL,
  check_command text default NULL,
  check_freshness boolean default NULL,
  check_interval double default NULL,
  check_period varchar(75) default NULL,
  check_type smallint default NULL,
  checked boolean default NULL,
  command_line text default NULL,
  default_active_checks boolean default NULL,
  default_event_handler_enabled boolean default NULL,
  default_failure_prediction boolean default NULL,
  default_flap_detection boolean default NULL,
  default_notify boolean default NULL,
  default_passive_checks boolean default NULL,
  default_process_perfdata boolean default NULL,
  display_name varchar(100) default NULL,
  event_handler varchar(255) default NULL,
  event_handler_enabled boolean default NULL,
  execution_time double default NULL,
  failure_prediction boolean default NULL,
  first_notification_delay double default NULL,
  flap_detection boolean default NULL,
  flap_detection_on_down boolean default NULL,
  flap_detection_on_unreachable boolean default NULL,
  flap_detection_on_up boolean default NULL,
  flapping boolean default NULL,
  freshness_threshold double default NULL,
  high_flap_threshold double default NULL,
  icon_image varchar(255) default NULL,
  icon_image_alt varchar(255) default NULL,
  initial_state varchar(18) default NULL,
  last_check int default NULL,
  last_hard_state smallint default NULL,
  last_hard_state_change int default NULL,
  last_notification int default NULL,
  last_state_change int default NULL,
  last_time_down int default NULL,
  last_time_unreachable int default NULL,
  last_time_up int default NULL,
  last_update int default NULL,
  latency double default NULL,
  low_flap_threshold double default NULL,
  max_check_attempts smallint default NULL,
  modified_attributes int default NULL,
  next_check int default NULL,
  next_host_notification int default NULL,
  no_more_notifications boolean default NULL,
  notes varchar(255) default NULL,
  notes_url varchar(255) default NULL,
  notification_interval double default NULL,
  notification_number smallint default NULL,
  notification_period varchar(75) default NULL,
  notify boolean default NULL,
  notify_on_down boolean default NULL,
  notify_on_downtime boolean default NULL,
  notify_on_flapping boolean default NULL,
  notify_on_recovery boolean default NULL,
  notify_on_unreachable boolean default NULL,
  obsess_over_host boolean default NULL,
  output text default NULL,
  passive_checks boolean default NULL,
  percent_state_change double default NULL,
  perfdata text default NULL,
  process_perfdata boolean default NULL,
  retain_nonstatus_information boolean default NULL,
  retain_status_information boolean default NULL,
  retry_interval double default NULL,
  scheduled_downtime_depth smallint default NULL,
  should_be_scheduled boolean default NULL,
  stalk_on_down boolean default NULL,
  stalk_on_unreachable boolean default NULL,
  stalk_on_up boolean default NULL,
  state smallint default NULL,
  state_type smallint default NULL,
  statusmap_image varchar(255) default NULL,

  UNIQUE (host_id),
  UNIQUE (host_name, instance_id),
  FOREIGN KEY (instance_id) REFERENCES instances (instance_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Host groups.
--
CREATE TABLE hostgroups (
  hostgroup_id int NOT NULL auto_increment,
  name varchar(255) NOT NULL,
  instance_id int NOT NULL,

  action_url varchar(160) default NULL,
  alias varchar(255) default NULL,
  notes varchar(160) default NULL,
  notes_url varchar(160) default NULL,

  PRIMARY KEY (hostgroup_id),
  UNIQUE (name, instance_id),
  FOREIGN KEY (instance_id) REFERENCES instances (instance_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Relationships between hosts and host groups.
--
CREATE TABLE hosts_hostgroups (
  host_id int NOT NULL,
  hostgroup_id int NOT NULL,

  UNIQUE (host_id, hostgroup_id),
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (hostgroup_id) REFERENCES hostgroups (id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Hosts parenting relationships.
--
CREATE TABLE host_parent (
  host_id int NOT NULL,
  parent_id int NOT NULL,

  UNIQUE (host_id, parent_id),
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (parent_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Hosts dependencies.
--
CREATE TABLE host_dependency (
  id int NOT NULL auto_increment,
  dependent_host_id int NOT NULL,
  host_id int NOT NULL,

  dependency_period varchar(75) default NULL,
  execution_failure_options varchar(15) default NULL,
  inherits_parent boolean default NULL,
  notification_failure_options varchar(15) default NULL,

  PRIMARY KEY (id),
  UNIQUE (dependent_host_id, host_id),
  FOREIGN KEY (dependent_host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Correlated issues.
--
CREATE TABLE issues (
  issue_id int NOT NULL auto_increment,
  host_id int default NULL,
  service_id int default NULL,
  start_time int NOT NULL,

  ack_time int default NULL,
  end_time int default NULL,

  PRIMARY KEY (issue_id),
  UNIQUE (host_id, service_id, start_time)
) ENGINE=InnoDB;


--
-- Issues parenting.
--
CREATE TABLE issue_parent (
  child_issue_id int NOT NULL,
  end_time int default NULL,
  start_time int NOT NULL,
  parent_issue_id int NOT NULL,

  FOREIGN KEY (child_issue_id) REFERENCES issues (issue_id)
    ON DELETE CASCADE,
  FOREIGN KEY (parent_issue_id) REFERENCES issues (issue_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Nagios logs.
--
CREATE TABLE logs (
  id int NOT NULL auto_increment,

  ctime int default NULL,
  host_name varchar(255) default NULL,
  instance_name varchar(255) NOT NULL,
  issue_id int default NULL,
  msg_type tinyint default NULL,
  notification_cmd varchar(255) default NULL,
  notification_contact varchar(255) default NULL,
  output text default NULL,
  retry int default NULL,
  service_description varchar(255) default NULL,
  status enum('0', '1', '2', '3', '4') default NULL,
  type smallint default NULL,

  PRIMARY KEY (id)
) ENGINE=MyISAM;


--
-- Downtimes.
--
CREATE TABLE downtimes (
  id int NOT NULL auto_increment,
  entry_time int default NULL,
  instance_name varchar(255) NOT NULL,
  internal_id int default NULL,

  author_name varchar(64) default NULL,
  comment_data text default NULL,
  downtime_type smallint default NULL,
  duration int default NULL,
  end_time int default NULL,
  fixed boolean default NULL,
  host_name varchar(255) NOT NULL,
  service_description varchar(255) default NULL,
  start_time int default NULL,
  triggered_by int default NULL,
  was_cancelled boolean default NULL,
  was_started boolean default NULL,

  PRIMARY KEY (id),
  UNIQUE (entry_time, instance_name, internal_id)
) ENGINE=InnoDB;


--
-- Monitored services.
--
CREATE TABLE services (
  host_id int NOT NULL,
  service_description varchar(255) NOT NULL,
  service_id int default NULL,

  acknowledgement_type smallint default NULL,
  action_url varchar(255) default NULL,
  active_checks_enabled boolean default NULL,
  check_command text default NULL,
  check_freshness boolean default NULL,
  check_interval double default NULL,
  check_period varchar(75) default NULL,
  check_type smallint default NULL,
  command_line text default NULL,
  current_attempt smallint default NULL,
  current_notification_number smallint default NULL,
  current_state smallint default NULL,
  default_active_checks_enabled boolean default NULL,
  default_event_handler_enabled boolean default NULL,
  default_failure_prediction boolean default NULL,
  default_flap_detection_enabled boolean default NULL,
  default_notifications_enabled boolean default NULL,
  default_passive_checks_enabled boolean default NULL,
  default_process_perfdata boolean default NULL,
  display_name varchar(160) default NULL,
  event_handler varchar(255) default NULL,
  event_handler_enabled boolean default NULL,
  execution_time double default NULL,
  failure_prediction_enabled boolean default NULL,
  failure_prediction_options varchar(64) default NULL,
  first_notification_delay double default NULL,
  flap_detection_enabled boolean default NULL,
  flap_detection_on_critical boolean default NULL,
  flap_detection_on_ok boolean default NULL,
  flap_detection_on_unknown boolean default NULL,
  flap_detection_on_warning boolean default NULL,
  freshness_threshold double default NULL,
  has_been_checked boolean default NULL,
  high_flap_threshold double default NULL,
  icon_image varchar(255) default NULL,
  icon_image_alt varchar(255) default NULL,
  initial_state varchar(1) default NULL,
  is_flapping boolean default NULL,
  is_volatile boolean default NULL,
  last_check int default NULL,
  last_hard_state smallint default NULL,
  last_hard_state_change int default NULL,
  last_notification int default NULL,
  last_state_change int default NULL,
  last_time_critical int default NULL,
  last_time_ok int default NULL,
  last_time_unknown int default NULL,
  last_time_warning int default NULL,
  last_update int default NULL,
  latency double default NULL,
  low_flap_threshold double default NULL,
  max_check_attempts smallint default NULL,
  modified_attributes int default NULL,
  next_check int default NULL,
  next_notification int default NULL,
  no_more_notifications boolean default NULL,
  notes varchar(255) default NULL,
  notes_url varchar(255) default NULL,
  notification_interval double default NULL,
  notification_period varchar(75) default NULL,
  notifications_enabled boolean default NULL,
  notify_on_critical boolean default NULL,
  notify_on_downtime boolean default NULL,
  notify_on_flapping boolean default NULL,
  notify_on_recovery boolean default NULL,
  notify_on_unknown boolean default NULL,
  notify_on_warning boolean default NULL,
  obsess_over_service boolean default NULL,
  output text default NULL,
  passive_checks_enabled boolean default NULL,
  percent_state_change double default NULL,
  perf_data text default NULL,
  problem_has_been_acknowledged boolean default NULL,
  process_perfdata boolean default NULL,
  retain_nonstatus_information boolean default NULL,
  retain_status_information boolean default NULL,
  retry_interval double default NULL,
  scheduled_downtime_depth smallint default NULL,
  should_be_scheduled boolean default NULL,
  stalk_on_critical boolean default NULL,
  stalk_on_ok boolean default NULL,
  stalk_on_unknown boolean default NULL,
  stalk_on_warning boolean default NULL,
  state_type smallint default NULL,

  UNIQUE (host_id, service_description),
  UNIQUE (service_id),
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Groups of services.
--
CREATE TABLE servicegroups (
  id int NOT NULL auto_increment,
  instance_id int NOT NULL,
  servicegroup_name varchar(255) NOT NULL,

  action_url varchar(160) default NULL,
  alias varchar(255) default NULL,
  notes varchar(160) default NULL,
  notes_url varchar(160) default NULL,

  PRIMARY KEY (id),
  FOREIGN KEY (instance_id) REFERENCES instances (instance_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Relationships between services and service groups.
--
CREATE TABLE service_servicegroup (
  service_id int NOT NULL,
  servicegroup_id int NOT NULL,

  UNIQUE (service_id, servicegroup_id),
  FOREIGN KEY (service_id) REFERENCES services (service_id)
    ON DELETE CASCADE,
  FOREIGN KEY (servicegroup_id) REFERENCES servicegroups (id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Services dependencies.
--
CREATE TABLE service_dependency (
  id int NOT NULL auto_increment,
  dependent_service_id int NOT NULL,
  service_id int NOT NULL,

  dependency_period varchar(75) default NULL,
  execution_failure_options varchar(15) default NULL,
  inherits_parent boolean default NULL,
  notification_failure_options varchar(15) default NULL,

  PRIMARY KEY (id),
  FOREIGN KEY (dependent_service_id) REFERENCES services (service_id)
    ON DELETE CASCADE,
  FOREIGN KEY (service_id) REFERENCES services (service_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Custom variables.
--
CREATE TABLE customvariables (
  id int NOT NULL auto_increment,

  config_type smallint default NULL,
  has_been_modified boolean default NULL,
  host_id int default NULL,
  service_id int default NULL,
  status_update_time int NOT NULL,
  varname varchar(255) default NULL,
  varvalue varchar(255) default NULL,

  PRIMARY KEY (id)
) ENGINE=InnoDB;


--
-- Event handlers.
--
CREATE TABLE eventhandlers (
  id int NOT NULL auto_increment,

  command_args varchar(255) default NULL,
  command_line varchar(255) default NULL,
  early_timeout smallint default NULL,
  end_time int default NULL,
  eventhandler_type smallint default NULL,
  execution_time double default NULL,
  host_id int default NULL,
  output varchar(255) default NULL,
  return_code smallint default NULL,
  service_id int default NULL,
  start_time int default NULL,
  state smallint default NULL,
  state_type smallint default NULL,
  timeout smallint default NULL,

  PRIMARY KEY (id)
) ENGINE=InnoDB;


--
--  Notifications.
--
CREATE TABLE notifications (
  id int NOT NULL auto_increment,

  ack_author varchar(255) default NULL,
  ack_data text default NULL,
  command_name varchar(255) default NULL,
  contact_name varchar(255) default NULL,
  contacts_notified boolean default NULL,
  end_time int default NULL,
  escalated boolean default NULL,
  host_id int default NULL,
  notification_type int default NULL,
  output text default NULL,
  reason_type int default NULL,
  service_id int default NULL,
  start_time int default NULL,
  state int default NULL,

  PRIMARY KEY (id)
) ENGINE=InnoDB;


--
-- Historization of flapping status.
--
CREATE TABLE flappinghistory (
  id int NOT NULL auto_increment,

  comment_time int default NULL,
  event_time int default NULL,
  event_type smallint default NULL,
  flapping_type smallint default NULL,
  high_threshold double default NULL,
  host_id int default NULL,
  instance_id int default NULL,
  internal_comment_id int default NULL,
  low_threshold double default NULL,
  percent_state_change double default NULL,
  reason_type smallint default NULL,
  service_id int default NULL,

  PRIMARY KEY (id)
) ENGINE=InnoDB;


--
-- Executed commands related to hosts.
--
CREATE TABLE hostcommands (
  host_id int default NULL,

  check_command text default NULL,
  event_handler_command text default NULL,

  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Executed commands related to services.
--
CREATE TABLE services_commands (
  service_id int default NULL,

  check_command text,
  event_handler_command text,

  FOREIGN KEY (service_id) REFERENCES services (service_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- States of checkpoints.
--
CREATE TABLE states (
  state_id int NOT NULL auto_increment,

  end_time int default NULL,
  host_id int NOT NULL,
  service_id int default NULL,
  start_time int NOT NULL,
  state int NOT NULL,
  
  PRIMARY KEY (state_id)
) ENGINE=InnoDB;
