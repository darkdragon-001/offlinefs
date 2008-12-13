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
CPPUNIT_TEST_SUITE_REGISTRATION(Test_FilesystemStatusManager);
//////////////////////////////////////////////////////

void Test_FilesystemStatusManager::TestInstance()
{
    // Sets the objects up and processes.
    FilesystemStatusManager fssm1 = FilesystemStatusManager::Instance();
    FilesystemStatusManager fssm2 = FilesystemStatusManager::Instance();

    // Checks the result.
    CPPUNIT_ASSERT(&fssm1 == &fssm2);
}
