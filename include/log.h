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

#ifndef __RILLOGGER__LOG__H__
#define __RILLOGGER__LOG__H__

#include <stdint.h>

typedef enum {
    // Android -> RIL calls
    EVENT_INIT            = 1,
    EVENT_REQUEST         = 2,
    EVENT_STATE_REQUEST   = 3,
    EVENT_SUPPORTS        = 4,
    EVENT_CANCEL          = 5,
    EVENT_GETVERSION      = 6,
    EVENT_TIMEDCALLBACK   = 7,
    
    // RIL -> Android calls
    EVENT_REQUESTCOMPLETE = 128,
    EVENT_UNSOL_RESPONSE  = 129,
    EVENT_REQUESTTIMED    = 130
} log_event_t;

typedef struct {
    uint32_t type;
    uint32_t data_size;
    uint64_t timestamp;
} log_entry_header_t;

typedef struct {
    log_entry_header_t hdr;
} init_log_entry_t;

typedef struct {
    log_entry_header_t hdr;
    uint32_t request;
    uint32_t token;
    unsigned char data[];
} request_log_entry_t;

typedef struct {
    log_entry_header_t hdr;
    uint32_t state;
} state_request_log_entry_t;

typedef struct {
    log_entry_header_t hdr;
    uint32_t request;
    uint32_t supports;
} supports_log_entry_t;

typedef struct {
    log_entry_header_t hdr;
    uint32_t token;
} cancel_log_entry_t;

typedef struct {
    log_entry_header_t hdr;
    char version[];
} get_version_log_entry_t;

typedef struct {
    log_entry_header_t hdr;
    uint32_t proc;
    uint32_t arg;
} timed_callback_log_entry_t;

typedef struct {
    log_entry_header_t hdr;
    uint32_t token;
    uint32_t req_errno;
    unsigned char data[];
} request_complete_log_entry_t;

typedef struct {
    log_entry_header_t hdr;
    uint32_t response;
    unsigned char data[];
} unsol_response_log_entry_t;

typedef struct {
    log_entry_header_t hdr;
    uint32_t proc;
    uint32_t arg;
    uint64_t timestamp;
} request_timed_callback_log_entry_t;

#endif
