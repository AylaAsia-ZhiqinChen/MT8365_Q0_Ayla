// #include "ICDCodecAPI.h"
// #include "ICDCodecApiEX.h"
#include "libmdmonitor.h"
#include <assert.h>
#include <string>
using namespace libmdmonitor;


enum ICDCodecApiReturnCode
{
    ICD_CODEC_RET_E_FILE_NOT_EXIST = -1000,
    ICD_CODEC_RET_E_DB_INVALID = -900,
    ICD_CODEC_RET_E_DB_STRUCT_UNDEFINED,
    ICD_CODEC_RET_E_DB_STRUCT_INVALID,
    ICD_CODEC_RET_E_DB_FIELD_TYPE_UNDEFINED,
    ICD_CODEC_RET_E_HEADER_INVALID_BUFFER_SIZE = -800,
    ICD_CODEC_RET_E_HEADER_UNSUPPORTED_TIME_TYPE,
    ICD_CODEC_RET_E_HEADER_ICD_TYPE_UNDEFINED,
    ICD_CODEC_RET_E_HEADER_CHECKSUM_ERROR,
    ICD_CODEC_RET_E_PAYLOAD_ICD_CODE_UNDEFINED = -700,
    ICD_CODEC_RET_E_PAYLOAD_INVALID_BUFFER_SIZE,
    ICD_CODEC_RET_E_PAYLOAD_FIELD_UNDEFINED,
    ICD_CODEC_RET_E_PAYLOAD_FIELD_VALUE_OUT_OF_RANGE,
    ICD_CODEC_RET_E_OUTPUT_BUFFER_SIZE_TOO_SMALL = - 600,

    ICD_CODEC_RET_E_INVALID_ARGUMENTS = -500,
    ICD_CODEC_RET_E_NOT_SUPPORT,
    ICD_CODEC_RET_E_FAIL = 0,
    ICD_CODEC_RET_S_OK = 1
};

typedef struct
{
    unsigned char type:4;               //4bit
    unsigned char version:4;            //4bit
    unsigned short total_len;           //16bit
    unsigned char timestamp_type:4;     //4bit
    unsigned char protocol_id:4;        //4bit
    unsigned short event_code;          //16bit
    unsigned short header_checksum;     //16bit
    unsigned long long timestamp;       //1 or 2 or 4 or 8 byte
    unsigned int size;  //an integer to record the actually byte-usage of this ICD packet
} IcdEventHeader;

typedef struct
{
    unsigned char type:4;               //4bit
    unsigned char version:4;            //4bit
    unsigned int total_len:24;      //24bit
    unsigned char timestamp_type:4; //4bit
    unsigned char protocol_id:4;    //4bit
    unsigned char reserved;         //8bit
    unsigned short record_code;     //16bit
    unsigned int header_checksum;   //32bit
    unsigned long long timestamp;   //8byte or 4 byte
    unsigned int size;  //an integer to record the actually byte-usage of this ICD packet
} IcdRecordHeader;

class IcdDecoder
{
public:
    // IcdDecoder();
    IcdDecoder();
    virtual ~IcdDecoder();

    ICDCodecApiReturnCode GetICDVersion(const unsigned char *packet, unsigned int &version);
    ICDCodecApiReturnCode GetICDType(const unsigned char *packet, unsigned int &type);
    ICDCodecApiReturnCode GetICDTimestamp(const unsigned char *packet, const unsigned int packetSize, unsigned long long &timestamp);
    ICDCodecApiReturnCode DecodeICDPacketHeader(const unsigned char *packet, const unsigned int packetSize, unsigned int &code, unsigned char &protocolID, char &token, const unsigned char **payload, unsigned int &payloadSize);
    ICDCodecApiReturnCode DecodeICDEventPacketHeader(const unsigned char *buffer, const unsigned int bufferLen, IcdEventHeader *outICDEventHeader);
    ICDCodecApiReturnCode DecodeICDRecordPacketHeader(const unsigned char *buffer, const unsigned int bufferLen, IcdRecordHeader *outICDRecordHeader);

    bool ParseTrapInfo(TRAP_TYPE trapType, uint64_t rtosTime, const unsigned char *pData, size_t len, FRAME_INFO &frameInfo, const unsigned char *&payload, uint16_t &frameNumber);
    // bool GetICDName(const unsigned int code, std::string &name);
};