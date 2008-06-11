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
#include "backingtreepersistence.h"
#include <iostream>
#include <sstream>
using namespace std;

std::auto_ptr<BackingtreePersistence> BackingtreePersistence::theBackingtreePersistenceInstance;
Mutex BackingtreePersistence::m;

BackingtreePersistence::BackingtreePersistence() :
	PersistenceManager(PERSISTENCE_MODULE_NAME) {}
BackingtreePersistence::~BackingtreePersistence(){}

BackingtreePersistence& BackingtreePersistence::Instance()
{
    MutexLocker obtain_lock(m);
    if (theBackingtreePersistenceInstance.get() == 0) {
    	theBackingtreePersistenceInstance.reset(new BackingtreePersistence());
        theBackingtreePersistenceInstance->init();
    }
    return *theBackingtreePersistenceInstance;
}


cfg_opt_t *BackingtreePersistence::init_parser()
{
	cfg_opt_t *opts = new cfg_opt_t[2];
	opts[0] = (cfg_opt_t)CFG_STR_LIST(CONFIGKEY_BACKINGTREES, "{}", CFGF_NONE);
	opts[1] = (cfg_opt_t)CFG_END();

	return opts;
}

string BackingtreePersistence::get_persistence()
{
	stringstream pers;
	pers << CONFIGKEY_BACKINGTREES << " = {" << endl;
	list<Backingtree>::iterator it;
	bool first = true;
	for ( it=p_backingtrees.begin() ; it != p_backingtrees.end(); it++ ) {
		if(first)
			first = false;
		else
			pers << "," << endl;
		pers << "\"" << it->get_relative_path() << "\"";
	}
	pers << endl << "}" << endl;
	return pers.str();
}


list<Backingtree> BackingtreePersistence::backingtrees()
{
	reload();
	return p_backingtrees;
}

void BackingtreePersistence::backingtrees(const list<Backingtree> backingt)
{
	p_backingtrees = backingt;
	make_persistent();
}

void BackingtreePersistence::read_values()
{
	p_backingtrees.clear();
	for(int i = 0; i < cfg_size(cfg, CONFIGKEY_BACKINGTREES); i++) {
		p_backingtrees.push_back(Backingtree(string(
			cfg_getnstr(cfg, CONFIGKEY_BACKINGTREES, i))));
	}
}
