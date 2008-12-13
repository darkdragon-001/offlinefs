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
#ifndef OFSPROPERTIESDLG_H
#define OFSPROPERTIESDLG_H

#include "OFS_Properties.h"

/**
    @author Frank Gsellmann <frank.gsellmann@gmx.de>
*/

enum NETWORK_CONN_LOST_ACTION
{
    NCLA_NONE = -1,
    NCLA_NOTIFY_ME,
    NCLA_NEVER_ALLOW_GO_OFFLINE
};

class OFSPropertiesDlg : public OFSProperties
{
    Q_OBJECT
public:
    OFSPropertiesDlg(QWidget* pParent = 0, const char* pszName = 0, bool bModal = false, WFlags fl = 0);
    ~OFSPropertiesDlg();
protected slots:
    void OnDeleteFiles();
    void OnViewFiles();
    void OnAdvanced();
protected:
    NETWORK_CONN_LOST_ACTION m_nNetworkConnLostAction;
//    int m_nNetworkConnLostAction;
};

#endif  // !OFSPROPERTIESDLG_H
