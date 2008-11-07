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
CPPUNIT_TEST_SUITE_REGISTRATION(Test_SyncLogEntry);
//////////////////////////////////////////////////////

void Test_SyncLogEntry::TestConstructor()
{
    // Sets up
	const string strFilePath = "aaa/bbb";
	const string strModTime = "1234/56/78 25:99:99";
	const char chModType = 'q';

    // Processes
    SyncLogEntry sle(strFilePath, strModTime, chModType);

    // Checks the values.
    CPPUNIT_ASSERT_EQUAL(strFilePath, sle.GetFilePath());
    CPPUNIT_ASSERT_EQUAL(strFilePath.c_str(), sle.GetFilePath().c_str());
    CPPUNIT_ASSERT(strcmp(strFilePath.c_str(), sle.GetFilePath().c_str()) == 0);
    CPPUNIT_ASSERT_EQUAL(strModTime, sle.GetModTime());
    CPPUNIT_ASSERT_EQUAL(strModTime.c_str(), sle.GetModTime().c_str());
    CPPUNIT_ASSERT(strcmp(strModTime.c_str(), sle.GetModTime().c_str()) == 0);
    CPPUNIT_ASSERT_EQUAL(chModType, sle.GetModType());
}

void Test_SyncLogEntry::TestEqual()
{
	// Set up
	const string strA = "aaa";
	const string strB = "bbb";
	const string str1 = "111";
	const string str2 = "222";

	const SyncLogEntry sleA1a(strA, str1, a);
	const SyncLogEntry sleB1a(strB, str1, a);
	const SyncLogEntry sleA2a(strA, str2, a);
	const SyncLogEntry sleB2a(strB, str2, a);
	const SyncLogEntry sleA1b(strA, str1, b);
	const SyncLogEntry sleB1b(strB, str1, b);
	const SyncLogEntry sleA2b(strA, str2, b);
	const SyncLogEntry sleB2b(strB, str2, b);

	// Process & Check
	CPPUNIT_ASSERT(sleA1a == strA);
	CPPUNIT_ASSERT(sleB1a != strA);
	CPPUNIT_ASSERT(sleA2a == strA);
	CPPUNIT_ASSERT(sleB2a != strA);
	CPPUNIT_ASSERT(sleA1b == strA);
	CPPUNIT_ASSERT(sleB1b != strA);
	CPPUNIT_ASSERT(sleA2b == strA);
	CPPUNIT_ASSERT(sleB2b != strA);
}
