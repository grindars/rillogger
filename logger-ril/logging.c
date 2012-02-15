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

#include <android/log.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "logging.h"

static int logfile = -1;

int logfile_open(const char *filename) {
    logfile = open(filename, O_WRONLY | O_APPEND | O_CREAT, 0644);
    
    if(logfile == -1)
        return -1;
    
    return 0;
}

void logfile_close(void) {
    if(logfile != -1) {
        close(logfile);
        
        logfile = -1;
    }
}

void log_event(log_event_t event, void *data, size_t size) {
    if(logfile == -1) {
        __android_log_write(ANDROID_LOG_WARN, "RIL", "logger-ril: log file not open");        
        
        return;
    }
    
    struct timeval time;
    gettimeofday(&time, NULL);
    
    log_entry_header_t *hdr = (log_entry_header_t *) data;
    hdr->type = event;
    hdr->data_size = size - sizeof(log_entry_header_t);
    hdr->timestamp = timeval_to_timestamp(&time);
    
    if(write(logfile, data, size) == -1) {
        __android_log_print(ANDROID_LOG_WARN, "RIL", "logger-ril: event write failed: %s", strerror(errno));   
    }
}

uint64_t timeval_to_timestamp(const struct timeval *val) {
    return val->tv_sec * 1000 + (val->tv_usec / 1000);
}
