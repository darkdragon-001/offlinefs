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
#include "persistencemanager.h"
#include "ofshash.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

PersistenceManager::PersistenceManager(string modname)
{
	stringstream str;
	unsigned char sha1hash[20];
	string hash;

	this->modname = modname;	

	// TODO: put the remote path here
	str << PERSISTENCE_PATH << "/" << ofs_hash("foobar") << "_" << modname;
	filename = str.str();
}

PersistenceManager::~PersistenceManager()
{
	cfg_free(cfg);
	delete opts;
}

void PersistenceManager::reload()
{
	if(cfg_parse(cfg, get_filename().c_str()) == CFG_PARSE_ERROR) {
		cfg_parse_buf(cfg, "");
	}
	read_values();
}

void PersistenceManager::make_persistent()
{
	ofstream persfile(get_filename().c_str(), ios::out | ios::trunc);
	persfile << get_persistence();
	persfile << endl;
	persfile.close();
}

string PersistenceManager::get_filename()
{
	return filename;
}

void PersistenceManager::init()
{
	opts = init_parser();
	cfg = cfg_init(opts, CFGF_NONE);
	reload();
}

string PersistenceManager::get_modname()
{
	return modname;
}
