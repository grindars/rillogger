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

#include <log.h>
#include <stdlib.h>
#include <string.h>

#include "wrappers.h"
#include "logging.h"

typedef struct {
    RIL_TimedCallback callback;
    void *param;
} timed_callback_data_t;

extern const RIL_RadioFunctions *wrappedRIL;
extern const struct RIL_Env *wrappedAndroid;

void wrapRequest(int request, void *data, size_t datalen, RIL_Token t) {
    request_log_entry_t *entry = malloc(sizeof(request_log_entry_t) + datalen);
    entry->request = request;
    memcpy(entry + 1, data, datalen);
    entry->token = (uint32_t) t;
    log_event(EVENT_REQUEST, entry, sizeof(request_log_entry_t) + datalen);
    free(entry);
    
    wrappedRIL->onRequest(request, data, datalen, t);
}

RIL_RadioState wrapStateRequest(void) {
    RIL_RadioState state = wrappedRIL->onStateRequest();
    
    state_request_log_entry_t *entry = malloc(sizeof(state_request_log_entry_t));
    entry->state = state;
    log_event(EVENT_STATE_REQUEST, entry, sizeof(state_request_log_entry_t));
    free(entry);
    
    return state;
}

int wrapSupports(int requestCode) {
    int supports = wrappedRIL->supports(requestCode);
    
    supports_log_entry_t *entry = malloc(sizeof(supports_log_entry_t));
    entry->request = requestCode;
    entry->supports = supports;
    log_event(EVENT_SUPPORTS, entry, sizeof(supports_log_entry_t));
    free(entry);    
    
    return supports;
}

void wrapCancel(RIL_Token t) {
    cancel_log_entry_t *entry = malloc(sizeof(cancel_log_entry_t));
    entry->token = (uint32_t) t;
    log_event(EVENT_CANCEL, entry, sizeof(cancel_log_entry_t));
    free(entry);
    
    wrappedRIL->onCancel(t);
}

const char *wrapGetVersion(void) {
    const char *version = wrappedRIL->getVersion();
    size_t len = strlen(version);
    
    get_version_log_entry_t *entry = malloc(sizeof(get_version_log_entry_t) + len);
    memcpy(entry + 1, version, len);
    log_event(EVENT_GETVERSION, entry, sizeof(get_version_log_entry_t) + len);
    free(entry);
    
    return version;
}

void wrapRequestComplete(RIL_Token t, RIL_Errno e, void *response, size_t responselen) {
    request_complete_log_entry_t *entry = malloc(sizeof(request_complete_log_entry_t) + responselen);
    entry->token = (uint32_t) t;
    entry->req_errno = e;
    memcpy(entry + 1, response, responselen);
    log_event(EVENT_REQUESTCOMPLETE, entry, sizeof(request_complete_log_entry_t) + responselen);
    free(entry);
    
    wrappedAndroid->OnRequestComplete(t, e, response, responselen);
}

void wrapUnsolicitedResponse(int unsolResponse, const void *data, size_t datalen) {
    unsol_response_log_entry_t *entry = malloc(sizeof(unsol_response_log_entry_t) + datalen);
    entry->response = unsolResponse;
    memcpy(entry + 1, data, datalen);
    log_event(EVENT_UNSOL_RESPONSE, entry, sizeof(unsol_response_log_entry_t) + datalen);
    free(entry);
    
    wrappedAndroid->OnUnsolicitedResponse(unsolResponse, data, datalen);
}

static void wrapTimedCallback(void *param) {
    timed_callback_data_t *data = param;
    
    timed_callback_log_entry_t *entry = malloc(sizeof(timed_callback_log_entry_t));
    entry->proc = (uint32_t) data->callback;
    entry->arg = (uint32_t) data->param;
    log_event(EVENT_TIMEDCALLBACK, entry, sizeof(timed_callback_log_entry_t));
    free(entry);
    
    data->callback(data->param);
    free(data);    
}

void wrapRequestTimedCallback(RIL_TimedCallback callback, void *param, const struct timeval *relativeTime) {
    request_timed_callback_log_entry_t *entry = malloc(sizeof(request_timed_callback_log_entry_t));
    entry->proc = (uint32_t) callback;
    entry->arg = (uint32_t) param;
    entry->timestamp = relativeTime ? timeval_to_timestamp(relativeTime) : 0;
    log_event(EVENT_REQUESTTIMED, entry, sizeof(request_timed_callback_log_entry_t));
    free(entry);
    
    timed_callback_data_t *data = malloc(sizeof(timed_callback_data_t));
    data->callback = callback;
    data->param = param;
    
    wrappedAndroid->RequestTimedCallback(wrapTimedCallback, data, relativeTime);
}
