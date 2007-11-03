/***************************************************************************
 *   Copyright (C) 2007 by Carsten Kolassa   *
 *   Carsten@Kolassa.de   *
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
#ifndef OFS_FUSE_H
#define OFS_FUSE_H

#include <fusexx.hpp>
#include <string>

/**
	@author Carsten Kolassa <Carsten@Kolassa.de>
*/
class ofs_fuse : public fusexx::fuse<ofs_fuse> 
{
public:
    ofs_fuse();

    ~ofs_fuse();
    // Overload the fuse methods
    static int getattr (const char *, struct stat *);
                static int readdir (const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *);
                static int open (const char *, struct fuse_file_info *);
                static int read (const char *, char *, size_t, off_t, struct fuse_file_info *);
       
        private:
                // Private variables
                // Notice that they aren't static, i.e. they belong to an instantiated object
                std::string m_strHelloWorld;
                std::string m_strPath;
};

#endif
