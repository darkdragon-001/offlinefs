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
    	theBackingtreePersistenceInstance.reset(new BackingtreePersistence);
        theBackingtreePersistenceInstance->init();
    }
    return *theBackingtreePersistenceInstance;
}

/**
 * Set the parser options
 */
cfg_opt_t *BackingtreePersistence::init_parser()
{
	cfg_opt_t *opts = new cfg_opt_t[2];
	opts[0] = (cfg_opt_t)CFG_STR_LIST(CONFIGKEY_BACKINGTREES, "{}", CFGF_NONE);
	opts[1] = (cfg_opt_t)CFG_END();

	return opts;
}

/**
 * get the string which has to be written to the persistence file
 */
string BackingtreePersistence::get_persistence()
{
	stringstream pers;
	pers << CONFIGKEY_BACKINGTREES << " = {" << endl;
	list<string>::iterator it;
	for ( it=p_backingtrees.begin() ; it != p_backingtrees.end(); it++ )
		pers << "\"" << *it << "\"" << endl;
	pers << "}" << endl;
	return pers.str();
}

/**
 * get a list of backingtrees
 */
list<string> BackingtreePersistence::backingtrees() const
{
	return p_backingtrees;
}

/**
 * set the list of backingtrees and make persistent
 */
void BackingtreePersistence::backingtrees(const list<string> backingt)
{
	p_backingtrees = backingt;
}

/**
 * read the config values
 */
void BackingtreePersistence::read_values()
{
	p_backingtrees.clear();
	for(int i = 0; i < cfg_size(cfg, CONFIGKEY_BACKINGTREES); i++) {
		p_backingtrees.push_back(string(
			cfg_getnstr(cfg, CONFIGKEY_BACKINGTREES, i)));
	}
}