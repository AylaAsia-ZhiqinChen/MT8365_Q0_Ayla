#ifndef __APPINSIGHT_H__
#define __APPINSIGHT_H__

#include <stdio.h>
#include <time.h>
#include <string>
#include <cstring>
#include <array>
#include <memory>

#include <cutils/properties.h>
#include <android/log.h>

#ifndef LOG_TAG
#define LOG_TAG "AppInsight"
#endif

#define APPINSIGHT_LOGD(B, ...) if(B) __android_log_print(ANDROID_LOG_INFO,LOG_TAG, __VA_ARGS__)

#define INFO_DUMP_GET_SYSTEM_PROP(TYPE, VAR, KEY, DEFAULT) \
    {\
        char value[PROPERTY_VALUE_MAX]; \
        property_get(KEY, value, DEFAULT); \
        VAR = (TYPE) atoi(value); \
    }

#define KEYLIST \
    keyType(SCENARIO), \
    keyType(APPNAME), \
    keyType(CODECTYPE), \
    keyType(WIDTH), \
    keyType(HEIGHT), \
    keyType(PIXEL_FORMAT), \
    keyType(BITRATE), \
    keyType(PROFILE), \
    keyType(LEVEL), \
    keyType(FRAMERATE), \
    keyType(GRID_WIDTH), \
    keyType(GRID_HEIGHT), \
    keyType(TILE_WIDTH), \
    keyType(TILE_HEIGHT), \
    keyType(PROGRESSIVE), \
    keyType(ICC_PROFILE), \
    keyType(COLOR_TYPE), \
    keyType(COLOR_SPACE), \
    keyType(SAMPLE_SIZE), \
    keyType(REGION_DECODE), \
    keyType(INFODUMPMAX)

#define TYPENONE(x) x
#define TYPESTR(x) #x

namespace AppInsight {

#define OUTPATH "/sdcard/appinsight"

#ifdef keyType
#undef keyType
#endif
#define keyType TYPENONE
enum InfoDumpKey {KEYLIST};

class InfoDump {
public:
    InfoDump(bool vendor=true)
    :modified(false),enabled(false),isVendor(vendor)
    {
        if(isVendor)
        {
            INFO_DUMP_GET_SYSTEM_PROP(bool, enabled, "vendor.mtk.appinsight.videoimage.enabled", "0");
        }
        else
        {
            INFO_DUMP_GET_SYSTEM_PROP(bool, enabled, "mtk.appinsight.videoimage.enabled", "0");
        }

        info.fill(NULL);
        infoType.fill(UNINIT);

        if(!enabled) return;

        char temp[512];
        if(isVendor)
            property_get("vendor.mtk.appinsight.videoimage.scenario", temp, "");
        else
            property_get("mtk.appinsight.videoimage.scenario", temp, "");

        if(temp[0]==0) { enabled = false; return; }
        else add(SCENARIO, temp);

        if(isVendor)
            property_get("vendor.mtk.appinsight.videoimage.appname", temp, "");
        else
            property_get("mtk.appinsight.videoimage.appname", temp, "");

        if(temp[0]==0) { enabled = false; return; }
        else add(APPNAME, temp);
    }
    ~InfoDump()
    {
        if(!enabled) return;
        for (int i=0;i<INFODUMPMAX; i++)
        {
            if(infoType[i] == UNINIT) continue;
            else if(info[i] != NULL) free(info[i]);
        }
    }

    bool add(InfoDumpKey k, const char* v)
    {
        if(!enabled) return false;
        if(k>=INFODUMPMAX) return false;

        int len = strlen(v) + 1;

        switch(infoType[k])
        {
            case STRING:
            {
                int orilen = *((int*)info[k]);
                if(orilen < len)
                {
                    free(info[k]);
                    info[k] = (char*)malloc(sizeof(int) + len);
                    *((int*)info[k]) = len;
                }
                break;
            }
            case NUMBER:
            case UNINIT:
            default:
                if(info[k]) free(info[k]);
                info[k] = (char*)malloc(sizeof(int) + len + 1);
                *((int*)info[k]) = len + 1;
        }

        memcpy(info[k]+4, v, len);
        infoType[k] = STRING;
        modified = true;
        return true;
    }

    bool add(InfoDumpKey k, int v)
    {
        if(!enabled) return false;
        if(k>=INFODUMPMAX) return false;

        switch(infoType[k])
        {
            case NUMBER:
                if(info[k] == NULL) info[k] = (char*)malloc(sizeof(int));
                if(*((int*)info[k]) == v) return false;
                break;
            case STRING:
            case UNINIT:
            default:
                if(info[k]) free(info[k]);
                info[k] = (char*)malloc(sizeof(int));
        }

        *((int*)info[k]) = v;
        infoType[k] = NUMBER;
        modified = true;
        return true;
    }

    bool add(InfoDumpKey k, std::string v)
    {
        if(!enabled) return false;
        return add(k, v.c_str());
    }

    void commit()
    {
        if(!enabled) return;
        if(!modified) return;

        time_t rawtime;
        struct tm * timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        if(timeinfo == nullptr) return;

        int loglen = 2048;
        int offset = 0;

        // auto delete sp
        auto logbuffer_sp = std::shared_ptr<char>((char*)malloc(loglen), [](char* ptr) {if(ptr) free(ptr);});
        if(logbuffer_sp == nullptr) return;

        char* logbuffer = logbuffer_sp.get();

        offset += snprintf(logbuffer, loglen-offset, "%s appinsight:", asctime(timeinfo));
        for ( int i=0; i<INFODUMPMAX; i++)
        {
            if(infoType[i] == UNINIT)
            {
                offset += snprintf(logbuffer+offset, loglen-offset, "%s:null,", InfoDumpKeyname[i]);
            }
            else if(infoType[i] == NUMBER)
            {
                offset += snprintf(logbuffer+offset, loglen-offset, "%s:%d,", InfoDumpKeyname[i], *((int*) info[i]));
            }
            else if(infoType[i] == STRING)
            {
                offset += snprintf(logbuffer+offset, loglen-offset, "%s:%s,", InfoDumpKeyname[i], info[i]+4);
            }
        }

        FILE* fout = fopen(OUTPATH, "a+");
        if(fout == NULL) return;

        fprintf(fout, "%s appinsight: %s\n", asctime(timeinfo), logbuffer);
        fclose(fout);

        printlog(logbuffer);

        modified = false;
    }

    void printlog(char* logbuffer)
    {
        if(!enabled) return;
        if(!modified) return;

        APPINSIGHT_LOGD(enabled, "%s", logbuffer);
    }

    bool enable()
    {
        return enabled;
    }

private:

    #ifdef keyType
    #undef keyType
    #endif
    #define keyType TYPESTR
    const char *InfoDumpKeyname[INFODUMPMAX+1] = {KEYLIST};

    enum InfoType { UNINIT, NUMBER, STRING };

    std::array<char*, INFODUMPMAX> info;
    std::array<InfoType, INFODUMPMAX> infoType;
    /*
    * memlayout:
    *   UNINIT: NULL
    *   NUMBER: char[4] <--(value int)
    *   STRING: char[4+LEN] <--(LEN int + value str)
    */

    bool modified;
    bool enabled;
    bool isVendor;
};

};

#endif
