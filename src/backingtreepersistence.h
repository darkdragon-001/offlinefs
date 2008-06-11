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
#ifndef BACKINGTREEPERSISTENCE_H
#define BACKINGTREEPERSISTENCE_H
#include "persistencemanager.h"
#include "mutexlocker.h"
#include "backingtree.h"
#include <list>
#define CONFIGKEY_BACKINGTREES "backingtrees"
#define PERSISTENCE_MODULE_NAME "backingtrees"

/**
 * @author Tobias Jaehnel <tjaehnel@gmail.com>
 * Defines a interface for the Backingtreemanager to make
 * the list of backingtrees persistent and reload them
 */
class BackingtreePersistence : public PersistenceManager {
public:
    /**
     * Get singleton instance
     * @return singleton instance
     */
    static BackingtreePersistence& Instance();
    ~BackingtreePersistence();
    /**
     * make backingtrees persistent
     * @param trees list of backingtrees
     */
    void backingtrees(const list<Backingtree> trees);
    /**
     * load backingtrees
     * @return list of backingtrees
     */
    list<Backingtree> backingtrees();
protected:
    /**
     * ctor
     */
    BackingtreePersistence();
    virtual cfg_opt_t * init_parser();
    virtual string get_persistence();
    virtual void read_values();

private:
    list<Backingtree> p_backingtrees;

    static std::auto_ptr<BackingtreePersistence> theBackingtreePersistenceInstance;
    static Mutex m;


};

#endif
