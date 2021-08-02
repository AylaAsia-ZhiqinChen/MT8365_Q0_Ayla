#ifndef _CFG_YUNTIAN_PRO_INFO_FILE_H
#define _CFG_YUNTIAN_PRO_INFO_FILE_H

///Yuntian - Added to extend NvRAM &&{{
typedef struct
{
    unsigned char reserved[1024*16];
} yuntian_nvram;
///&&}}

typedef struct{
        yuntian_nvram yuntian;
}YUNTIAN_PRO_INFO;

//the record size and number of PRODUCT_INFO nvram file
#define CFG_FILE_YUNTIAN_PRO_INFO_SIZE    sizeof(YUNTIAN_PRO_INFO)
#define CFG_FILE_YUNTIAN_PRO_INFO_TOTAL   1

#endif /* _CFG_YUNTIAN_PRO_INFO_FILE_H */
