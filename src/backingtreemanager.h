/***************************************************************************
 *   Copyright (C) 2007 by Carsten Kolassa   *
 *   Carsten@Kolassa.de   *
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
#ifndef BACKINGTREEMANAGER_H
#define BACKINGTREEMANAGER_H
#include "mutexlocker.h"
#include "backingtree.h"
#include <string>
/**
	@author Carsten Kolassa <Carsten@Kolassa.de>
*/
class BackingtreeManager{
public:
    static BackingtreeManager& Instance();
    ~BackingtreeManager(); 
    void register_Backingtree(string MountPath, string TempMount);
    void remove_Backingtree(string MountPath, string TempMount);
protected:
    BackingtreeManager();
  private:
    static std::auto_ptr<BackingtreeManager> theBackingtreeManagerInstance;
    vector<Backingtree> backinglist;
    static Mutex m; 
};
#endif
