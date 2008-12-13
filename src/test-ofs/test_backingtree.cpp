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
CPPUNIT_TEST_SUITE_REGISTRATION(Test_Backingtree);
//////////////////////////////////////////////////////

void Test_Backingtree::TestConstructor()
{
    // Sets up
	const string strRelativePath = "aaa/bbb";
	const string strCachePath = "ccc/ddd";

    // Processes
    Backingtree bt(strRelativePath, strCachePath);

    // Checks the values.
    CPPUNIT_ASSERT_EQUAL(strRelativePath, bt.get_cache_path());
    CPPUNIT_ASSERT_EQUAL(strRelativePath.c_str(), bt.get_cache_path().c_str());
    CPPUNIT_ASSERT(strcmp(strRelativePath.c_str(), bt.get_cache_path().c_str()) == 0);
    CPPUNIT_ASSERT_EQUAL(strCachePath, bt.get_relative_path());
    CPPUNIT_ASSERT_EQUAL(strCachePath.c_str(), bt.get_relative_path().c_str());
    CPPUNIT_ASSERT(strcmp(strCachePath.c_str(), bt.get_relative_path().c_str()) == 0);
}

void Test_Backingtree::TestEqual()
{
	// Set up
	const Backingtree btA1("aaa", "111");
	const Backingtree btB1("bbb", "111");
	const Backingtree btA2("aaa", "222");
	const Backingtree btB2("bbb", "222");

	// Process & Check
	CPPUNIT_ASSERT(btA1 == btA1);	// ==
	CPPUNIT_ASSERT(btB1 != btA1);	// != relative path
	CPPUNIT_ASSERT(btA2 != btA1);   // != cache path
	CPPUNIT_ASSERT(btB2 != btA1);	// != relative path and != cache path
}

void Test_Backingtree::TestSearchPath()
{
	// Set up
	const Backingtree btBCD("bbb/ccc/ddd", "111");

	const string strLongPath = "aaa/bbb/ccc/ddd/eee";
	const string strShortPath = "ccc";

	// Process & Check
	CPPUNIT_ASSERT(btBCD.is_in_backingtree(strLongPath));
	CPPUNIT_ASSERT(!btBCD.backingtree_is_in(strLongPath));
	CPPUNIT_ASSERT(!btBCD.is_in_backingtree(strShortPath));
	CPPUNIT_ASSERT(btBCD.backingtree_is_in(strShortPath));
}

void Test_Backingtree::TestGetCachePath()
{
	// Set up
	const Backingtree btABC("aaa/bbb/ccc", "111");

	const string strWrongPath = "ddd/eee/fff/ggg";
	const string strShortPath = "aaa/bbb";
	const string strEqualPath = "aaa/bbb/ccc";
	const string strLongPath = "aaa/bbb/ccc/ddd/eee";

	// Process & Check
	CPPUNIT_ASSERT(btABC.get_cache_path(strWrongPath) == NULL);
	CPPUNIT_ASSERT(btABC.get_cache_path(strShortPath) == NULL);
	CPPUNIT_ASSERT_EQUAL(btABC.get_cache_path(strEqualPath), "111");
	CPPUNIT_ASSERT_EQUAL(btABC.get_cache_path(strLongPath), "111/ddd/eee");
}
