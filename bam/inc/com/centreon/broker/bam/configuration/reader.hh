/*
** Copyright 2009-2014 Merethis
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

#ifndef CCB_BAM_CONFIGURATION_READER_HH
#  define CCB_BAM_CONFIGURATION_READER_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/bam/configuration/db.hh"

CCB_BEGIN()

namespace       bam{
  namespace     configuration{
    // Class declarations.
    class db;
    class state;

    /**
     *  @class reader reader.hh "com/centreon/broker/bam/configuration/reader.hh"
     *
     *  @brief Using the dbinfo to access the database, load state_obj
     *         with configuration.
     *
     *  Extract the database content to a configuration state usable by
     *  the BAM engine.
     */
    class reader {
    public:
      reader(configuration::db const&);
      ~reader();
      void read(state& state_obj);

    private:
      // No copies or assignments
      reader(reader const&);
      reader& operator=(reader const&);
      void ensure_open();
      void load(state::kpis& kpis);
      void load(state::bas& bas);
      void load(state::bool_exps& bool_exps);
      void assert_query(QSqlQuery& queryToCheck);

      QSqlDatabase      _db;
      configuration::db _dbinfo;
    };
  }
}

CCB_END()

#endif // !CCB_BAM_CONFIGURATION_READER_HH
