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
CPPUNIT_TEST_SUITE_REGISTRATION(Test_SyncLogger);
//////////////////////////////////////////////////////

void Test_SyncLogger::TestInstance()
{
    // Sets the objects up and processes.
    SyncLogger logger1 = SyncLogger::Instance();
    SyncLogger logger2 = SyncLogger::Instance();

    // Checks the result.
    CPPUNIT_ASSERT(&logger1 == &logger2);
/*
	// Set up
	const double dReal1 = 13.07;
	const double dReal2 = 7.08;
	const double dImaginaer2 = 1234.5678;

	// Process
	CKomplex komplex;
	CKomplex komplex1(dReal1);
	CKomplex komplex2(dReal2, dImaginaer2);

	// Check
	CPPUNIT_ASSERT_EQUAL(0.0, komplex.GetRealTeil());
	CPPUNIT_ASSERT_EQUAL(0.0, komplex.GetImaginaerTeil());
	CPPUNIT_ASSERT_EQUAL(dReal1, komplex1.GetRealTeil());
	CPPUNIT_ASSERT_EQUAL(0.0, komplex1.GetImaginaerTeil());
	CPPUNIT_ASSERT_EQUAL(dReal2, komplex2.GetRealTeil());
	CPPUNIT_ASSERT_EQUAL(dImaginaer2, komplex2.GetImaginaerTeil());
*/
}
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
CPPUNIT_TEST_SUITE_REGISTRATION(Test_SyncLogger);
//////////////////////////////////////////////////////

void Test_SyncLogger::TestInstance()
{
    // Sets the objects up and processes.
    SyncLogger logger1 = SyncLogger::Instance();
    SyncLogger logger2 = SyncLogger::Instance();

    // Checks the result.
    CPPUNIT_ASSERT(&logger1 == &logger2);
/*
	// Set up
	const double dReal1 = 13.07;
	const double dReal2 = 7.08;
	const double dImaginaer2 = 1234.5678;

	// Process
	CKomplex komplex;
	CKomplex komplex1(dReal1);
	CKomplex komplex2(dReal2, dImaginaer2);

	// Check
	CPPUNIT_ASSERT_EQUAL(0.0, komplex.GetRealTeil());
	CPPUNIT_ASSERT_EQUAL(0.0, komplex.GetImaginaerTeil());
	CPPUNIT_ASSERT_EQUAL(dReal1, komplex1.GetRealTeil());
	CPPUNIT_ASSERT_EQUAL(0.0, komplex1.GetImaginaerTeil());
	CPPUNIT_ASSERT_EQUAL(dReal2, komplex2.GetRealTeil());
	CPPUNIT_ASSERT_EQUAL(dImaginaer2, komplex2.GetImaginaerTeil());
*/
}
