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

#include "test_ofsadvancedsettingsdlg.h"
#include <string>
using namespace std;

//////////////////////////////////////////////////////
/// do the registry of the test_fixture
//CPPUNIT_TEST_SUITE_REGISTRATION(Test_OFSAdvancedSettingsDlg);
//////////////////////////////////////////////////////

void Test_OFSAdvancedSettingsDlg::TestConstructor()
{
    // Sets up
    const string strMessage = "Error708";
    const int nPosixErrno = 708;

    // Processes
//    OFSAdvancedSettingsDlg dlg;
/*    OFSAdvancedSettingsDlg xcCopy(xc);

    // Checks the values.
    CPPUNIT_ASSERT_EQUAL(strMessage.c_str(), xc.what());
    CPPUNIT_ASSERT(strcmp(strMessage.c_str(), xc.what()) == 0);
    CPPUNIT_ASSERT_EQUAL(nPosixErrno, xc.get_posixerrno());
    CPPUNIT_ASSERT_EQUAL(strMessage.c_str(), xcCopy.what());
    CPPUNIT_ASSERT(strcmp(strMessage.c_str(), xcCopy.what()) == 0);
    CPPUNIT_ASSERT_EQUAL(nPosixErrno, xcCopy.get_posixerrno());*/
}
