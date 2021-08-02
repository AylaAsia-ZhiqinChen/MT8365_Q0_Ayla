/*
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the TRUSTONIC LIMITED nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** @addtogroup CMP
 * @{
 * @file
 * Interface to content management trustlet (TlCm) definitions.
 *
 * The TlCm is responsible for implementing content management protocol (CMP)
 * commands and generating approriate CMP responses in the trustlet control
 * interface (TCI).
 */

#ifndef TL_CM_API_H_
#define TL_CM_API_H_

#include "TlCm/tlCmApiCommon.h"
#include "TlCm/3.0/cmp.h"
#include "TlCm/3.0/cmpMap.h"

/** TCI CMP messages. */
typedef union {
    /** Command header. */
    cmpCommandHeaderTci_t commandHeader;
    /** Response header. */
    cmpResponseHeaderTci_t responseHeader;

    /** System command GetVersion. */
    cmpCmdGetVersionTci_t cmpCmdGetVersionTci;
    /** System response GetVersion. */
    cmpRspGetVersionTci_t cmpRspGetVersionTci;
    /** System command GetSuid. */
    cmpCmdGetSuidTci_t cmpCmdGetSuidTci;
    /** System response GetSuid. */
    cmpRspGetSuidTci_t cmpRspGetSuidTci;
    /** System command GenAuthToken. */
    cmpCmdGenAuthTokenTci_t cmpCmdGenAuthTokenTci;
    /** System response GenAuthToken. */
    cmpRspGenAuthTokenTci_t cmpRspGenAuthTokenTci;
    /** System command GenBindingKey. */
    cmpCmdGenBindingKeyTci_t cmpCmdGenBindingKeyTci;
    /** System response GenBindingKey. */
    cmpRspGenBindingKeyTci_t cmpRspGenBindingKeyTci;

    /** Authentication command BeginSocAuthentication. */
    cmpCmdBeginSocAuthenticationTci_t cmpCmdBeginSocAuthenticationTci;
    /** Authentication response BeginSocAuthentication. */
    cmpRspBeginSocAuthenticationTci_t cmpRspBeginSocAuthenticationTci;
    /** Authentication command BeginRootAuthentication. */
    cmpCmdBeginRootAuthenticationTci_t cmpCmdBeginRootAuthenticationTci;
    /** Authentication response BeginRootAuthentication. */
    cmpRspBeginRootAuthenticationTci_t cmpRspBeginRootAuthenticationTci;
    /** Authentication command BeginSpAuthentication. */
    cmpCmdBeginSpAuthenticationTci_t cmpCmdBeginSpAuthenticationTci;
    /** Authentication response BeginSpAuthentication. */
    cmpRspBeginSpAuthenticationTci_t cmpRspBeginSpAuthenticationTci;
    /** Authentication command Authenticate. */
    cmpCmdAuthenticateTci_t cmpCmdAuthenticateTci;
    /** Authentication response Authenticate. */
    cmpRspAuthenticateTci_t cmpRspAuthenticateTci;
    /** Authentication command AuthenticateTerminate. */
    cmpCmdAuthenticateTerminateTci_t cmpCmdAuthenticateTerminateTci;
    /** Authentication response AuthenticateTerminate. */
    cmpRspAuthenticateTerminateTci_t cmpRspAuthenticateTerminateTci;

    /** SoC administrative command RootContRegisterActivate. */
    cmpCmdRootContRegisterActivateTci_t cmpCmdRootContRegisterActivateTci;
    /** SoC administrative response RootContRegisterActivate. */
    cmpRspRootContRegisterActivateTci_t cmpRspRootContRegisterActivateTci;

    /** Root administrative command RootContUnregister. */
    cmpCmdRootContUnregisterTci_t cmpCmdRootContUnregisterTci;
    /** Root administrative response RootContUnregister. */
    cmpRspRootContUnregisterTci_t cmpRspRootContUnregisterTci;
    /** Root administrative command RootContLockByRoot. */
    cmpCmdRootContLockByRootTci_t cmpCmdRootContLockByRootTci;
    /** Root administrative response RootContLockByRoot. */
    cmpRspRootContLockByRootTci_t cmpRspRootContLockByRootTci;
    /** Root administrative command RootContUnlockByRoot. */
    cmpCmdRootContUnlockByRootTci_t cmpCmdRootContUnlockByRootTci;
    /** Root administrative command RootContUnlockByRoot. */
    cmpRspRootContUnlockByRootTci_t cmpRspRootContUnlockByRootTci;
    /** Root administrative command SpContRegisterActivate. */
    cmpCmdSpContRegisterActivateTci_t cmpCmdSpContRegisterActivateTci;
    /** Root administrative response SpContRegisterActivate. */
    cmpRspSpContRegisterActivateTci_t cmpRspSpContRegisterActivateTci;
    /** Root administrative command SpContUnregister. */
    cmpCmdSpContUnregisterTci_t cmpCmdSpContUnregisterTci;
    /** Root administrative response SpContUnregister. */
    cmpRspSpContUnregisterTci_t cmpRspSpContUnregisterTci;
    /** Root administrative command SpContRegister. */
    cmpCmdSpContRegisterTci_t cmpCmdSpContRegisterTci;
    /** Root administrative response SpContRegister. */
    cmpRspSpContRegisterTci_t cmpRspSpContRegisterTci;
    /** Root administrative command SpContLockByRoot. */
    cmpCmdSpContLockByRootTci_t cmpCmdSpContLockByRootTci;
    /** Root administrative response SpContLockByRoot. */
    cmpRspSpContLockByRootTci_t cmpRspSpContLockByRootTci;
    /** Root administrative command SpContUnlockByRoot. */
    cmpCmdSpContUnlockByRootTci_t cmpCmdSpContUnlockByRootTci;
    /** Root administrative response SpContUnlockByRoot. */
    cmpRspSpContUnlockByRootTci_t cmpRspSpContUnlockByRootTci;

    /** Sp administrative command SpContActivate. */
    cmpCmdSpContActivateTci_t cmpCmdSpContActivateTci;
    /** Sp administrative response SpContActivate. */
    cmpRspSpContActivateTci_t cmpRspSpContActivateTci;
    /** Sp administrative command SpContLockBySp. */
    cmpCmdSpContLockBySpTci_t cmpCmdSpContLockBySpTci;
    /** Sp administrative response SpContLockBySp. */
    cmpRspSpContLockBySpTci_t cmpRspSpContLockBySpTci;
    /** Sp administrative command SpContUnlockBySp. */
    cmpCmdSpContUnlockBySpTci_t cmpCmdSpContUnlockBySpTci;
    /** Sp administrative command SpContUnlockBySp. */
    cmpRspSpContUnlockBySpTci_t cmpRspSpContUnlockBySpTci;
    /** Sp administrative command TltContRegisterActivate. */
    cmpCmdTltContRegisterActivateTci_t cmpCmdTltContRegisterActivateTci;
    /** Sp administrative response TltContRegisterActivate. */
    cmpRspTltContRegisterActivateTci_t cmpRspTltContRegisterActivateTci;
    /** Sp administrative command TltContUnregister. */
    cmpCmdTltContUnregisterTci_t cmpCmdTltContUnregisterTci;
    /** Sp administrative response TltContUnregister. */
    cmpRspTltContUnregisterTci_t cmpRspTltContUnregisterTci;
    /** Sp administrative command TltContRegister. */
    cmpCmdTltContRegisterTci_t cmpCmdTltContRegisterTci;
    /** Sp administrative command TltContRegister. */
    cmpRspTltContRegisterTci_t cmpRspTltContRegisterTci;
    /** Sp administrative command TltContActivate. */
    cmpCmdTltContActivateTci_t cmpCmdTltContActivateTci;
    /** Sp administrative command TltContActivate. */
    cmpRspTltContActivateTci_t cmpRspTltContActivateTci;
    /** Sp administrative command TltContLockBySp. */
    cmpCmdTltContLockBySpTci_t cmpCmdTltContLockBySpTci;
    /** Sp administrative response TltContLockBySp. */
    cmpRspTltContLockBySpTci_t cmpRspTltContLockBySpTci;
    /** Sp administrative command TltContUnlockBySp. */
    cmpCmdTltContUnlockBySpTci_t cmpCmdTltContUnlockBySpTci;
    /** Sp administrative response TltContUnlockBySp. */
    cmpRspTltContUnlockBySpTci_t cmpRspTltContUnlockBySpTci;
    /** Sp administrative command TltContPersonalize. */
    cmpCmdTltContPersonalizeTci_t cmpCmdTltContPersonalizeTci;
    /** Sp administrative response TltContPersonalize. */
    cmpRspTltContPersonalizeTci_t cmpRspTltContPersonalizeTci;
} cmpMessage_t;

/** TCI CMP. */
typedef struct {
    /** TCI CMP messages. */
    cmpMessage_t msg;
} cmp_t;

#endif // TL_CM_API_H_

/** @} */
