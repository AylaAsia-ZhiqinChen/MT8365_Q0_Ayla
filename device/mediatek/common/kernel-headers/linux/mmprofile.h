#ifndef __MMPROFILE_H__
#define __MMPROFILE_H__

#include "mmprofile_static_event.h"


#define MMProfileEventNameMaxLen 31

typedef unsigned int MMP_Event;

typedef enum {
    MMProfileFlagStart = 1,
    MMProfileFlagEnd = 2,
    MMProfileFlagPulse = 4,
    MMProfileFlagEventSeparator = 8,
    MMProfileFlagSystrace = 0x80000000,
    MMProfileFlagMax = 0xFFFFFFFF
} MMP_LogType;

typedef enum {
    MMProfileMetaStringMBS = 1,
    MMProfileMetaStringWCS,
    MMProfileMetaStructure,
    MMProfileMetaBitmap,
    MMProfileMetaRaw,
    MMProfileMetaUser = 0x10000000,
    MMProfileMetaUserM4UReg,
    MMProfileMetaMax = 0xFFFFFFFF
} MMP_MetaDataType;

typedef enum {
    MMProfileBitmapRGB565 = 1,
    MMProfileBitmapRGB888,
    MMProfileBitmapRGBA8888,
    MMProfileBitmapBGR888,
    MMProfileBitmapBGRA8888,
    MMProfileBitmapMax = 0xFFFFFFFF
} MMP_PixelFormat;

typedef struct {
    unsigned int data1;         // data1 (user defined)
    unsigned int data2;         // data2 (user defined)
    MMP_MetaDataType data_type;  // meta data type
    unsigned int size;          // meta data size
    void *pData;                 // meta data pointer
} MMP_MetaData_t;

typedef struct {
    unsigned int data1;         // data1 (user defined)
    unsigned int data2;         // data2 (user defined)
    unsigned int struct_size;   // structure size (bytes)
    void *pData;                 // structure pointer
    char struct_name[32];        // structure name
} MMP_MetaDataStructure_t;

typedef struct {
    unsigned int data1;         // data1 (user defined)
    unsigned int data2;         // data2 (user defined)
    unsigned int width;         // image width
    unsigned int height;        // image height
    MMP_PixelFormat format;      // image pixel format
    unsigned int start_pos;     // start offset of image data (base on pData)
    unsigned int bpp;           // bits per pixel
    int pitch;         // image pitch (bytes per line)
    unsigned int data_size;     // image data size (bytes)
    unsigned int down_sample_x; // horizontal down sample rate (>=1)
    unsigned int down_sample_y; // vertical down sample rate (>=1)
    void *pData;                 // image buffer address
} MMP_MetaDataBitmap_t;

#endif
