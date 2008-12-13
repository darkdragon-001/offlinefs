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

//////////////////////////////////////////////////////
/// do the registry of the test_fixture
CPPUNIT_TEST_SUITE_REGISTRATION(Test_BackingtreeManager);
//////////////////////////////////////////////////////

void Test_BackingtreeManager::TestInstance()
{
    // Sets the objects up and processes.
    BackingtreeManager btm1 = BackingtreeManager::Instance();
    BackingtreeManager btm2 = BackingtreeManager::Instance();

    // Checks the result.
    CPPUNIT_ASSERT(&btm1 == &btm2);
}

void Test_BackingtreeManager::TestRegisterAndRemove()
{
    // Sets the objects up and processes.
    BackingtreeManager btm = BackingtreeManager::Instance();
	const string strPath1 = "aaa/bbb";
	const string strPath2 = "ccc/ddd";

    // Checks the result.
    CPPUNIT_ASSERT(!btm.Is_in_Backingtree(strPath1));
    CPPUNIT_ASSERT(!btm.Is_in_Backingtree(strPath2));

	btm.register_Backingtree(strPath1);

    CPPUNIT_ASSERT(btm.Is_in_Backingtree(strPath1));
    CPPUNIT_ASSERT(!btm.Is_in_Backingtree(strPath2));

	btm.register_Backingtree(strPath2);

    CPPUNIT_ASSERT(btm.Is_in_Backingtree(strPath1));
    CPPUNIT_ASSERT(btm.Is_in_Backingtree(strPath2));

	btm.remove_Backingtree(strPath1);

    CPPUNIT_ASSERT(!btm.Is_in_Backingtree(strPath1));
    CPPUNIT_ASSERT(btm.Is_in_Backingtree(strPath2));

	btm.remove_Backingtree(strPath2);

    CPPUNIT_ASSERT(!btm.Is_in_Backingtree(strPath1));
    CPPUNIT_ASSERT(!btm.Is_in_Backingtree(strPath2));
}
