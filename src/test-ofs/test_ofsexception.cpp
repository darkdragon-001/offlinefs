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
CPPUNIT_TEST_SUITE_REGISTRATION(Test_OFSException);
//////////////////////////////////////////////////////

void Test_OFSException::TestConstructor()
{
    // Sets up
    const string strMessage = "Error708";
    const int nPosixErrno = 708;

    // Processes
    OFSException xc(strMessage, nPosixErrno);
    OFSException xcCopy(xc);

    // Checks the values.
    CPPUNIT_ASSERT_EQUAL(strMessage.c_str(), xc.what());
    CPPUNIT_ASSERT(strcmp(strMessage.c_str(), xc.what()) == 0);
    CPPUNIT_ASSERT_EQUAL(nPosixErrno, xc.get_posixerrno());
    CPPUNIT_ASSERT_EQUAL(strMessage.c_str(), xcCopy.what());
    CPPUNIT_ASSERT(strcmp(strMessage.c_str(), xcCopy.what()) == 0);
    CPPUNIT_ASSERT_EQUAL(nPosixErrno, xcCopy.get_posixerrno());
}

void Test_OFSException::TestAssignment()
{
    // Sets up
    const string strMessage1 = "Error708";
    const int nPosixErrno1 = 708;

    const string strMessage2 = "Error1307";
    const int nPosixErrno2 = 1307;

    // Sets the objects up and processes.
    OFSException xc1(strMessage1, nPosixErrno1);
    OFSException xc2(strMessage2, nPosixErrno2);

    OFSException xc1= xc2;

    // Checks the values.
    CPPUNIT_ASSERT_EQUAL(xc1.what(), xc2.what());
    CPPUNIT_ASSERT(strcmp(xc1.what(), xc2.what()) == 0);
    CPPUNIT_ASSERT_EQUAL(strMessage2.c_str(), xc1.what());
    CPPUNIT_ASSERT(strcmp(strMessage2.c_str(), xc1.what()) == 0);
    CPPUNIT_ASSERT_EQUAL(strMessage2.c_str(), xc2.what());
    CPPUNIT_ASSERT(strcmp(strMessage2.c_str(), xc2.what()) == 0);
    CPPUNIT_ASSERT_EQUAL(xc1.get_posixerrno(), xc2.get_posixerrno());
    CPPUNIT_ASSERT_EQUAL(nPosixErrno2, xc1.get_posixerrno());
    CPPUNIT_ASSERT_EQUAL(nPosixErrno2, xc2.get_posixerrno());
}
