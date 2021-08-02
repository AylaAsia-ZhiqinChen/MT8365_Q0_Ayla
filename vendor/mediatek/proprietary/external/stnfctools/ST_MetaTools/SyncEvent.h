/******************************************************************************
 *
 *  Copyright (C) 2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  Synchronize two or more threads using a condition variable and a mutex.
 *
 ******************************************************************************/
#pragma once
#include "CondVar.h"
#include "Mutex.h"

// #define DEBUG_SYNC

#ifdef DEBUG_SYNC
#define MYLOG(...) DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(__VA_ARGS__)
#else
#define MYLOG(...)
#endif

class SyncEvent {
 public:
  SyncEvent() : mCounter(0) {}

  /*******************************************************************************
  **
  ** Function:        ~SyncEvent
  **
  ** Description:     Cleanup all resources.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  ~SyncEvent() {}

  /*******************************************************************************
  **
  ** Function:        start
  **
  ** Description:     Start a synchronization operation.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void start() {
    MYLOG("%s: %p acquiring %p", __FUNCTION__, (void*)pthread_self(), &mMutex);
    mMutex.lock();
    MYLOG("%s: %p acquired %p", __FUNCTION__, (void*)pthread_self(), &mMutex);
  }

  void start(bool rset) {
    MYLOG("%s: %p (%s) acquiring %p", __FUNCTION__, (void*)pthread_self(),
          rset ? "rset" : "-", &mMutex);
    mMutex.lock();
    MYLOG("%s: %p acquired %p", __FUNCTION__, (void*)pthread_self(), &mMutex);
    if (rset) {
      mCounter = 0;
    }
  }

  /*******************************************************************************
  **
  ** Function:        wait
  **
  ** Description:     Block the thread and wait for the event to occur.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void wait() {
    while (mCounter <= 0) {
      MYLOG("%s: %p start wait %p", __FUNCTION__, (void*)pthread_self(),
            &mMutex);
      mCondVar.wait(mMutex);
      MYLOG("%s: %p wakeup %p, counter=%d", __FUNCTION__, (void*)pthread_self(),
            &mMutex, mCounter);
    }
    mCounter--;
  }

  /*******************************************************************************
  **
  ** Function:        wait
  **
  ** Description:     Block the thread and wait for the event to occur.
  **                  millisec: Timeout in milliseconds.
  **
  ** Returns:         True if wait is successful; false if timeout occurs.
  **
  *******************************************************************************/
  bool wait(long millisec) {
    bool retVal = true;
    while (retVal && (mCounter <= 0)) {
      MYLOG("%s: %p start timedwait %p", __FUNCTION__, (void*)pthread_self(),
            &mMutex);
      retVal = mCondVar.wait(mMutex, millisec);
      MYLOG("%s: %p wakeup %p, counter=%d, %s", __FUNCTION__,
            (void*)pthread_self(), &mMutex, mCounter,
            retVal ? "true" : "false");
    }
    if (retVal) {
      mCounter--;
    }
    return retVal;
  }

  /*******************************************************************************
  **
  ** Function:        notifyOne
  **
  ** Description:     Notify a blocked thread that the event has occured.
  *Unblocks it.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void notifyOne() {
    // we should lock the mutex if needed here. Assume this is always called
    // with mutex ownership at the moment.
    mCounter++;
    MYLOG("%s: %p increased to %d and signal %p", __FUNCTION__,
          (void*)pthread_self(), mCounter, &mMutex);
    mCondVar.notifyOne();
  }

  /*******************************************************************************
  **
  ** Function:        end
  **
  ** Description:     End a synchronization operation.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void end() {
    MYLOG("%s: %p releasing %p, counter=%d", __FUNCTION__,
          (void*)pthread_self(), &mMutex, mCounter);
    mMutex.unlock();
  }

  void reset() {
    MYLOG("%s: %p reset cnt %p, counter=%d", __FUNCTION__,
          (void*)pthread_self(), &mMutex, mCounter);
    mCounter = 0;
  }

 private:
  CondVar mCondVar;
  Mutex mMutex;
  int mCounter;
};

/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************
**
**  Name:           SyncEventGuard
**
**  Description:    Automatically start and end a synchronization event.
**
*****************************************************************************/
class SyncEventGuard {
 public:
  /*******************************************************************************
  **
  ** Function:        SyncEventGuard
  **
  ** Description:     Start a synchronization operation.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  SyncEventGuard(SyncEvent& event) : mEvent(event) {
    event.start();  // automatically start operation
  };

  /*******************************************************************************
  **
  ** Function:        ~SyncEventGuard
  **
  ** Description:     End a synchronization operation.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  ~SyncEventGuard() {
    mEvent.end();  // automatically end operation
  };

 private:
  SyncEvent& mEvent;
};
