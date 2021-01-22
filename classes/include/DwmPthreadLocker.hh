//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmPthreadLocker.hh 8401 $
// @(#) $Id: DwmPthreadLocker.hh 8401 2016-04-17 06:44:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2008, 2016
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//  3. The names of the authors and copyright holders may not be used to
//     endorse or promote products derived from this software without
//     specific prior written permission.
//
//  IN NO EVENT SHALL DANIEL W. MCROBB BE LIABLE TO ANY PARTY FOR
//  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
//  INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE,
//  EVEN IF DANIEL W. MCROBB HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
//
//  THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND
//  DANIEL W. MCROBB HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
//  UPDATES, ENHANCEMENTS, OR MODIFICATIONS. DANIEL W. MCROBB MAKES NO
//  REPRESENTATIONS AND EXTENDS NO WARRANTIES OF ANY KIND, EITHER
//  IMPLIED OR EXPRESS, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE,
//  OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmPthreadLocker.hh
//!  \brief Dwm::Pthread::Locker class template definition
//---------------------------------------------------------------------------

#ifndef _DWMPTHREADLOCKER_HH_
#define _DWMPTHREADLOCKER_HH_

#include <map>

#include "DwmMutex.hh"

namespace Dwm {

  namespace Pthread {

    typedef std::map<const char *, std::pair<uint32_t,Mutex> > LockerMutexMap;
    extern Mutex            g_LockerMutex;
    extern LockerMutexMap   g_LockerMutexes;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    class Locker
    {
    public:
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      Locker(const T *obj)
      {
        _obj = (const char *)obj;
        g_LockerMutex.Lock();
        LockerMutexMap::iterator  iter = g_LockerMutexes.find(_obj);
        if (iter == g_LockerMutexes.end()) {
          g_LockerMutexes[_obj].first = 1;
        }
        else {
          ++iter->second.first;
        }
        g_LockerMutex.Unlock();
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      ~Locker()
      {
        g_LockerMutex.Lock();
        LockerMutexMap::iterator  iter = g_LockerMutexes.find(_obj);
        if (iter != g_LockerMutexes.end()) {
          --iter->second.first;
          if (! iter->second.first) {
            g_LockerMutexes.erase(iter);
          }
        }
        g_LockerMutex.Unlock();
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool Lock()
      {
        bool  rc = false;
        g_LockerMutex.Lock();
        LockerMutexMap::iterator  iter = g_LockerMutexes.find(_obj);
        if (iter != g_LockerMutexes.end()) {
          rc = iter->second.second.Lock();
        }
        g_LockerMutex.Unlock();
        return(rc);
      }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool Unlock()
      {
        bool rc = false;
        g_LockerMutex.Lock();
        LockerMutexMap::iterator  iter = g_LockerMutexes.find(_obj);
        if (iter != g_LockerMutexes.end()) {
          rc = iter->second.second.Unlock();
        }
        g_LockerMutex.Unlock();
        return(rc);
      }
      
    private:
      const char  *_obj;
    };

  }  // namespace Pthread

}  // namespace Dwm

#endif  // _DWMPTHREADLOCKER_HH_
