/***************************************************************************
 *   Copyright (C) 2008 by                                                 *
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

#include "ofsbroadcast.h"
#include "ofslog.h"
#include <cstdlib>
#include <memory>
#include <unistd.h>

// Initializes the class attributes.
std::auto_ptr<OFSBroadcast> OFSBroadcast::theOFSBroadcastInstance;
Mutex OFSBroadcast::m_mutex;

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION/ DESTRUCTION
//////////////////////////////////////////////////////////////////////////////

OFSBroadcast::OFSBroadcast()
{
}

OFSBroadcast::~OFSBroadcast()
{
}

OFSBroadcast& OFSBroadcast::Instance()
{
    MutexLocker obtainLock(m_mutex);
    if (theOFSBroadcastInstance.get() == 0)
    {
        theOFSBroadcastInstance.reset(new OFSBroadcast);
    }

    return *theOFSBroadcastInstance;
}



/**
 * Connect to the DBUS bus and send a broadcast signal
 */
void OFSBroadcast::SendSignal(const char* pszSignal,const char* pszValue,int nValue)
{

   return; ///\todo skipping DBUS stuff by now
   DBusMessage* msg;
   DBusMessageIter args;
   DBusConnection* conn;
   DBusError err;
   int ret;
   dbus_uint32_t serial = 0;

   ofslog::debug("Sending signal %s with value %s\n", pszSignal, pszValue);

   // initialise the error value
   dbus_error_init(&err);

   // connect to the DBUS system bus, and check for errors
   conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
   if (dbus_error_is_set(&err)) { 
      ofslog::error("DBus Connection Error (%s)",err.message);
      dbus_error_free(&err); 
   }
   if (NULL == conn) { 
      exit(1); 
   }

   // register our name on the bus, and check for errors
   ret = dbus_bus_request_name(conn, "test.signal.source", DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
   if (dbus_error_is_set(&err)) { 
      ofslog::error("DBus Name Error (%s)",err.message);
      dbus_error_free(&err); 
   }
   if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) { 
      exit(1);
   }

   // create a signal & check for errors 
   msg = dbus_message_new_signal("/ofs/signal/Object", // object name of the signal
                                 "ofs.signal.Type", // interface name of the signal
                                 pszSignal); // name of the signal
   if (NULL == msg) 
   { 
      ofslog::error("DBus invalid message.");
      exit(1); 
   }

   // append arguments onto signal
   dbus_message_iter_init_append(msg, &args);
   if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &pszValue)) {
      ofslog::error("DBus out of memory.");
      exit(1);
   }

   if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &nValue)) {
      ofslog::error("DBus out of memory.");
      exit(1);
   }
   // send the message and flush the connection
   if (!dbus_connection_send(conn, msg, &serial)) {
      ofslog::error("DBus out of memory.");
      exit(1);
   }
   dbus_connection_flush(conn);
   
   ofslog::debug("DBus signal sent.");
   
   // free the message and close the connection
   dbus_message_unref(msg);
   dbus_connection_close(conn);
}

/**
 * Connect to the DBUS bus and send a broadcast signal
 */
void OFSBroadcast::SendInfo(const char* pszSignal,const char* pszValue,const char* pszDesc,int nValue)
{
    ofslog::info(pszDesc);
    SendSignal(pszSignal,pszValue,nValue);
}

/**
 * Connect to the DBUS bus and send a broadcast signal
 */
void OFSBroadcast::SendError(const char* pszSignal,const char* pszValue,const char* pszDesc,int nValue)
{
    ofslog::error(pszDesc);
    SendSignal(pszSignal,pszValue,nValue);
}

/**
 * Server that exposes a method call and waits for it to be called
 */
void OFSBroadcast::Listen() 
{
   return; ///\todo skipping DBUS stuff by now
   DBusMessage* msg;
   DBusConnection* conn;
   DBusError err;
   int ret;

   ofslog::debug("DBus Listening for method calls.");

   // initialise the error
   dbus_error_init(&err);
   
   // connect to the bus and check for errors
   conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
   if (dbus_error_is_set(&err)) { 
	   ofslog::error("DBus Connection Error (%s)", err.message); 
      dbus_error_free(&err); 
   }
   if (NULL == conn) {
	   ofslog::error("DBus Connection invalid."); 
      exit(1); 
   }
   
   // request our name on the bus and check for errors
   ret = dbus_bus_request_name(conn, "ofs.method.server", DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
   if (dbus_error_is_set(&err)) { 
      ofslog::error("DBus Name Error (%s)", err.message); 
      dbus_error_free(&err);
   }
   if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) { 
	   ofslog::error("DBus Not Primary Owner (%d)", ret);
      exit(1); 
   }

   // loop, testing for new messages
   while (true) {
      // non blocking read of the next available message
      dbus_connection_read_write(conn, 0);
      msg = dbus_connection_pop_message(conn);

      // loop again if we haven't got a message
      if (NULL == msg) { 
         sleep(1);
         continue; 
      }
      
      // check this is a method call for the right interface & method
      if (dbus_message_is_method_call(msg, "ofs.method.Type", "Reintegrate"))
      {
         // TODO: Get method arguments and invoke SyncMan method Reintegrate.

         ReplyToMethodCall(msg, conn);
      }

      // free the message
      dbus_message_unref(msg);
   }

   // close the connection
   dbus_connection_close(conn);
}

void OFSBroadcast::ReplyToMethodCall(DBusMessage* msg, DBusConnection* conn)
{
   DBusMessage* reply;
   DBusMessageIter args;
   bool stat = true;
   dbus_uint32_t level = 21614;
   dbus_uint32_t serial = 0;
   char* param = "";

   // read the arguments
   if (!dbus_message_iter_init(msg, &args))
      ofslog::error("DBus Message has no arguments!"); 
   else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) 
      ofslog::error("DBus Argument is not string!"); 
   else 
      dbus_message_iter_get_basic(&args, &param);

   ofslog::debug("DBus method called with %s", param);
   

   // create a reply from the message
   reply = dbus_message_new_method_return(msg);

   // add the arguments to the reply
   dbus_message_iter_init_append(reply, &args);
   if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &stat)) { 
      ofslog::error("DBus out of memory.");
      exit(1);
   }
   if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &level)) { 
      ofslog::error("DBus out of memory.");
      exit(1);
   }

   // send the reply && flush the connection
   if (!dbus_connection_send(conn, reply, &serial)) {
      ofslog::error("DBus out of memory.");
      exit(1);
   }
   dbus_connection_flush(conn);

   // free the reply
   dbus_message_unref(reply);
}

