/*
 * Samsung RIL logging utility.
 * Copyright (C) 2012  Sergey Gridasov <grindars@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __WRAPPERS__H__
#define __WRAPPERS__H__

#include <telephony/ril.h>
#include <stdlib.h>
#include <string.h>

// RIL API wrappers
void wrapRequest(int request, void *data, size_t datalen, RIL_Token t);
RIL_RadioState wrapStateRequest(void);
int wrapSupports(int requestCode);
void wrapCancel(RIL_Token t);
const char *wrapGetVersion(void);

// Android API wrappers
void wrapRequestComplete(RIL_Token t, RIL_Errno e, void *response, size_t responselen);
void wrapUnsolicitedResponse(int unsolResponse, const void *data, size_t datalen);
void wrapRequestTimedCallback(RIL_TimedCallback callback, void *param, const struct timeval *relativeTime);

#endif
