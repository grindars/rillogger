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
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>

#include "logging.h"
#include "wrappers.h"

typedef const RIL_RadioFunctions *(*RIL_Init_t)(const struct RIL_Env *env, int argc, char **argv);

const RIL_RadioFunctions *wrappedRIL;
const struct RIL_Env *wrappedAndroid;

static const RIL_RadioFunctions RIL_wrappers = {
    .version        = 3,
    .onRequest      = wrapRequest,
    .onStateRequest = wrapStateRequest,
    .supports       = wrapSupports,
    .onCancel       = wrapCancel,
    .getVersion     = wrapGetVersion
};

static const struct RIL_Env android_wrappers = {
    .OnRequestComplete = wrapRequestComplete,
    .OnUnsolicitedResponse = wrapUnsolicitedResponse,
    .RequestTimedCallback = wrapRequestTimedCallback
};

static int parse_cmdline(int argc, char **argv, char **ril_path, char **log_file, int *last_optind) {
    int ch, ret = 0;
    *ril_path = NULL;
    *log_file = NULL;
    
    char *save_optarg = optarg;
    int save_optind = optind, save_opterr = opterr, save_optopt = optopt;
    optarg = NULL;
    optind = 1;
    opterr = 0;
    optopt = 0;
    
    
    while((ch = getopt(argc, argv, "r:l:")) != -1) {
        switch(ch) {
            case 'r':
                *ril_path = optarg;
                
                break;
                
            case 'l':
                *log_file = optarg;
                
                break;
                
            default:
                ret = -1;
                
                break;
        }
    }
    
    *last_optind = optind;    
    optarg = save_optarg;
    optind = save_optind;
    opterr = save_opterr;
    optopt = save_optopt;
    
    if(ret == -1 || *ril_path == NULL || *log_file == NULL)
        return -1;
    else
        return 0;
}

const RIL_RadioFunctions *RIL_Init(const struct RIL_Env *env, int argc, char **argv) {
    char *ril_path, *log_file;
    int optind;
    
    if(parse_cmdline(argc, argv, &ril_path, &log_file, &optind) == -1) {
        __android_log_write(ANDROID_LOG_ERROR, "RIL", "logger-ril: command line is incorrect. Usage: logger-ril.so -r <ORIGINAL RIL PATH> -l <LOG FILE PATH> [-- original ril args]");
        
        return NULL;
    }
    
    wrappedAndroid = env;
    
    if(logfile_open(log_file) == -1) {
        __android_log_print(ANDROID_LOG_ERROR, "RIL", "logger-ril: open of logfile %s failed: %s", log_file, strerror(errno));
        
        return NULL;        
    }
    
    void *original = dlopen(ril_path, RTLD_NOW);
    
    if(original == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "RIL", "logger-ril: dlopen of RIL %s failed: %s", ril_path, dlerror());
        
        return NULL;  
    }
    
    char **wrappedArgs = malloc(argc - optind + 1);
    wrappedArgs[0] = ril_path;
    memcpy(wrappedArgs + 1, argv + optind, (argc - optind) * sizeof(char *));
    
    RIL_Init_t wrappedRILInit = (RIL_Init_t) dlsym(original, "RIL_Init");
    if(wrappedRILInit == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "RIL", "logger-ril: dlsym of RIL_Init failed: %s", dlerror());
        
        free(wrappedArgs);
        
        return NULL;  
    }
     
    wrappedRIL = wrappedRILInit(wrappedAndroid, argc - optind + 1, wrappedArgs);
    // it's not safe to deallocate argv now.
    
    if(wrappedRIL == NULL)
        return NULL;
    
    init_log_entry_t *entry = malloc(sizeof(init_log_entry_t));
    log_event(EVENT_INIT, entry, sizeof(init_log_entry_t));
    free(entry);
    
    __android_log_print(ANDROID_LOG_INFO, "RIL", "logger-ril: operational; writing to %s", log_file);
    
    return &RIL_wrappers;
}
