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

#include <syslog.h>
#include <cstdio>
#include <cstdarg>
#include "ofslog.h"
#include "ofsconf.h"

#define MAX_LOGENTRY_LEN        1024

/*!
    \fn ofslog::init() 
 */
bool
ofslog::init()
{
    bool initok = true;
    openlog("ofs", LOG_PID|LOG_CONS|LOG_NDELAY, LOG_USER);

    OFSConf &ofsconf = OFSConf::Instance();
    int loglvl = ofsconf.GetLogLevel();
    int mask = LOG_UPTO(loglvl);
    setlogmask(mask);
    
    return initok;
}

/*!
    \fn ofslog::log(int loglvl,const char *fmt,va_list ap)
 */
void 
ofslog::log(int loglvl,const char *fmt,va_list ap)
{
    char buf[MAX_LOGENTRY_LEN];
    int len = vsnprintf(buf,MAX_LOGENTRY_LEN,fmt,ap);
    
    syslog(loglvl,"%s",buf);
}

/*!
    \fn ofslog::info(const char* msg)
 */
void 
ofslog::info(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log(LOG_INFO,fmt,args);
    va_end(args);
}

/*!
    \fn ofslog::debug(const char* msg)
 */
void 
ofslog::debug(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log(LOG_DEBUG,fmt,args);
    va_end(args);
}

/*!
    \fn ofslog::error(const char* msg)
 */
void 
ofslog::error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log(LOG_ERR,fmt,args);
    va_end(args);
}

/*!
    \fn ofslog::warning(const char* msg)
 */
void 
ofslog::warning(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log(LOG_WARNING,fmt,args);
    va_end(args);
}

/*!
    \fn ofslog::notice(const char* msg)
 */
void 
ofslog::notice(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log(LOG_NOTICE,fmt,args);
    va_end(args);
}

/*!
    \fn ofslog::critical(const char* msg)
 */
void 
ofslog::critical(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log(LOG_CRIT,fmt,args);
    va_end(args);
}


