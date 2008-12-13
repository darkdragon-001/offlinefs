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
#ifndef OFSCACHINGSETTINGSDLG_H
#define OFSCACHINGSETTINGSDLG_H

/**
    @author Frank Gsellmann <frank.gsellmann@gmx.de>
*/
class OFSCachingSettingsDlg : public OFS_CachingSettings
{
public:
    OFSCachingSettingsDlg(QWidget* pParent = 0, const char* pszName = 0, bool bModal = false, WFlags fl = 0);
    ~OFSCachingSettingsDlg();
protected:
};

#endif  // !OFSCACHINGSETTINGSDLG_H
