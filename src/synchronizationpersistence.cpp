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
#include "synchronizationpersistence.h"
#include <time.h>
#include <stdlib.h>
#include <sstream>
using namespace std;

std::auto_ptr<SynchronizationPersistence> SynchronizationPersistence::theSynchronizationPersistenceInstance;
Mutex SynchronizationPersistence::m;

SynchronizationPersistence::SynchronizationPersistence() :
    PersistenceManager(PERSISTENCE_MODULE_NAME)
{
}


SynchronizationPersistence::~SynchronizationPersistence()
{
}

SynchronizationPersistence& SynchronizationPersistence::Instance()
{
    MutexLocker obtain_lock(m);
    if (theSynchronizationPersistenceInstance.get() == 0) {
    	theSynchronizationPersistenceInstance.reset(new SynchronizationPersistence());
        theSynchronizationPersistenceInstance->init();
    }
    return *theSynchronizationPersistenceInstance;
}


cfg_opt_t *SynchronizationPersistence::init_parser()
{
	cfg_opt_t *opts = new cfg_opt_t[2];
	opts[0] = (cfg_opt_t)CFG_STR_LIST(
		CONFIGKEY_MTIMES, "{}", CFGF_NONE);
	opts[1] = (cfg_opt_t)CFG_END();
	return opts;
}

string SynchronizationPersistence::get_persistence()
{
	stringstream pers;
	pers << CONFIGKEY_MTIMES << " = {" << endl;
	map<string,time_t>::iterator it;
	bool first = true;
	for ( it=mtimemap.begin() ; it != mtimemap.end(); it++ ) {
		if(first)
			first = false;
		else
			pers << "," << endl;
		pers << "\"" << it->first << "\"," << endl;
		pers << it->second;
	}
	pers << endl << "}" << endl;
	return pers.str();
}


map<string,time_t> SynchronizationPersistence::mtimes()
{
	reload();
	return mtimemap;
}

void SynchronizationPersistence::mtimes(const map<string,time_t> modtimes)
{
	mtimemap = modtimes;
	make_persistent();
}

void SynchronizationPersistence::read_values()
{
    string relpath;
    string mtimestr;
    time_t mtime;
    mtimemap.clear();
    for(int i = 0; i < cfg_size(cfg, CONFIGKEY_MTIMES); i+=2) {
        relpath = string(cfg_getnstr(cfg, CONFIGKEY_MTIMES, i));
        mtimestr = atol(cfg_getnstr(cfg, CONFIGKEY_MTIMES, i+1));
        mtime = atol(mtimestr.c_str());
        mtimemap.insert(pair<string,time_t>(relpath, mtime));
    }
}
