#ifndef BRKS_COMMON_EVENT_TYPE_H
#define BRKS_COMMON_EVENT_TYPE_H
#include "glo_def.h"

typedef struct EErrorReason_
{
    i32 code;
    const char* reason;
}EErrorReason;

//事件ID
enum EventID
{
    EEVENTID_GET_MOBLIE_CODE_REQ     = 0x01,
    EEVENTID_GET_MOBLIE_CODE_RSP     = 0x02,

    EEVENTID_LOGIN_REQ               = 0x03,
    EEVENTID_LOGIN_RSP               = 0x04,

    EEVENTID_RECHARGE_REQ            = 0x05,
    EEVENTID_RECHARGE_RSP            = 0x06,

    EEVENTID_GET_ACCOUNT_BALANCE_REQ = 0x07,
    EEVENTID_ACCOUNT_BALANCE_RSP     = 0x08,

    EEVENTID_LIST_ACCOUNT_RECORDS_REQ = 0x09,
    EEVENTID_ACCOUNT_RECORDS_RSP      = 0x10,

    EEVENT_LIST_TRAVELS_REQ            = 0x11,
    EEVENT_LIST_TRAVELS_RSP            = 0x12,

    EEVENT_EXIT_RSP                    = 0xFE,
    EEVENTID_UNKOWN                    = 0xFF

};
//错误代号
enum EErrorCode
{
    ERRC_SUCCESS        = 200,
    ERRC_INVALID_MSG    = 400,
    ERRC_INVALID_DATA   = 404,
    ERRC_METHOD_NOT_ALLOWED = 405,
    ERRO_PROCESS_FAILED     = 406,
    ERRO_BIKE_IS_TOOK       = 407,
    ERRO_BIKE_IS_RUNNING    = 408,
    ERRO_BIKE_IS_DAMAGED    = 409,
    ERR_NULL                = 0 //undefine
};

const char* getReasonByErrorCode(i32 code);

#endif //BRKS_COMMON_EVENT_TYPE_H