/***************************************************************************
 *   Copyright (C) 2007 by                                                 *
 *                 Frank Gsellmann, Tobias Jaehnel, Carsten Kolassa        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "conflictpersistence.h"
#include <sstream>
using namespace std;

std::auto_ptr<ConflictPersistence> ConflictPersistence::theConflictPersistenceInstance;
Mutex ConflictPersistence::m;

ConflictPersistence::ConflictPersistence()
 : PersistenceManager(PERSISTENCE_MODULE_NAME)
{
}


ConflictPersistence::~ConflictPersistence()
{
}


ConflictPersistence& ConflictPersistence::Instance()
{
    MutexLocker obtain_lock(m);
    if (theConflictPersistenceInstance.get() == 0) {
    	theConflictPersistenceInstance.reset(new ConflictPersistence());
        theConflictPersistenceInstance->init();
    }
    return *theConflictPersistenceInstance;
}

cfg_opt_t *ConflictPersistence::init_parser()
{
	cfg_opt_t *opts = new cfg_opt_t[2];
	opts[0] = (cfg_opt_t)CFG_STR_LIST(
		CONFIGKEY_CFILES, "{}", CFGF_NONE);
	opts[1] = (cfg_opt_t)CFG_END();
	return opts;
}

string ConflictPersistence::get_persistence()
{
	stringstream pers;
	pers << CONFIGKEY_CFILES << " = {" << endl;
	list<string>::iterator it;
	bool first = true;
	for ( it=conflictfiles.begin() ; it != conflictfiles.end(); it++ ) {
		if(first)
			first = false;
		else
			pers << "," << endl;
		pers << "\"" << *it << "\"";
	}
	pers << endl << "}" << endl;
	return pers.str();
}

void ConflictPersistence::read_values()
{
    string relpath;
    conflictfiles.clear();
    for(int i = 0; i < cfg_size(cfg, CONFIGKEY_CFILES); i+=2) {
        relpath = string(cfg_getnstr(cfg, CONFIGKEY_CFILES, i));
        conflictfiles.push_back(relpath);
    }
}

void ConflictPersistence::files(const list<string> cfiles)
{
    conflictfiles = cfiles;
    make_persistent();
}

list<string> ConflictPersistence::files()
{
    reload();
    return conflictfiles;
}
