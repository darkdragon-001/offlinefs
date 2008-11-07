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

#include <qobject.h>
#include <qpushbutton.h>
#include <qradiobutton.h>

#include "ofspropertiesdlg.h"

#include "ofsconfirmfiledeletedlg.h"
#include "ofsadvancedsettingsdlg.h"

/*
enum NETWORK_CONN_LOST_ACTION
{
    NCLA_NONE = -1,
    NCLA_NOTIFY_ME,
    NCLA_NEVER_ALLOW_GO_OFFLINE
};
*/

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION/ DESTRUCTION
//////////////////////////////////////////////////////////////////////////////

OFSPropertiesDlg::OFSPropertiesDlg(QWidget* pParent, const char* pszName,
                                   bool bModal, WFlags fl)
    : OFSProperties(pParent, pszName, bModal, fl)
{
    connect(m_pbtnDeleteFiles, SIGNAL(clicked()), this, SLOT(OnDeleteFiles()));
    connect(m_pbtnAdvanced, SIGNAL(clicked()), this, SLOT(OnAdvanced()));
//    QObject::connect(m_pbtnAdvanced, SIGNAL(pressed()), this, SLOT(OnAdvanced()));
}

OFSPropertiesDlg::~OFSPropertiesDlg()
{
}

void OFSPropertiesDlg::OnDeleteFiles()
{
    OFSConfirmFileDeleteDlg dlg;
    dlg.exec();
    if (dlg.result() == Accepted)
    {
    }
}

void OFSPropertiesDlg::OnViewFiles()
{
}

void OFSPropertiesDlg::OnAdvanced()
{
    OFSAdvancedSettingsDlg dlg;
    dlg.m_prbNotifyMe->setChecked(m_nNetworkConnLostAction == NCLA_NOTIFY_ME);
    dlg.m_prbNeverAllowGoOffline->setChecked(m_nNetworkConnLostAction == NCLA_NEVER_ALLOW_GO_OFFLINE);
    dlg.exec();
    if (dlg.result() == Accepted)
    {
/*        if (dlg.m_prbNotifyMe->checked())
            m_nNetworkConnLostAction = NCLA_NOTIFY_ME;
        else if (dlg.m_prbNeverAllowGoOffline->checked())
            m_nNetworkConnLostAction = NCLA_NEVER_ALLOW_GO_OFFLINE;*/
    }
}
