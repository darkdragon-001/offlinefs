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
#ifndef SYNCSTATETYPE_H
#define SYNCSTATETYPE_H

/**
	@author Carsten Kolassa <Carsten@Kolassa.de>
*/
typedef enum syncstateenum
{
    no_state_avail           = 0,
    filesystem_not_available = 1,
    changed_on_server        = 2,
    changed_on_cache         = 3,
    changed_on_both_sides    = 4,
    unchanged                = 5,
    deleted_on_server        = 6,
    deleted_on_cache         = 7
} syncstate;

#endif
