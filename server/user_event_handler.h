#ifndef BRKS_BUS_USERM_HANDLER_H_
#define BRKS_BUS_USERM_HANDLER_H_

#include "glo_def.h"
#include "iEventHandler.h"
#include "events_def.h"
#include "threadpool/thread.h"
#include <string>
#include <map>
#include <memory>
#include "DispatchMsgService.h"
#include "configdef.h"
#include "Logger.h"
#include "iniconfig.h"
#include "sqlconnection.h"
#include "SqlTables.h"
#include "user_service.h"
class UserEventHandler : public iEventHandler
{
public:
    UserEventHandler();
    virtual ~UserEventHandler();
    virtual iEvent* handle(const iEvent* ev);
private:
    MobileCodeRspEv* handle_mobile_code_req(MobileCodeReqEv* ev);
    LoginRspEv* handle_login_req(LoginReqEv* ev);
    i32 code_gen();
private:
    std::map<std::string,i32> m2c_;
    pthread_mutex_t pm_;

};


#endif //BRKS_BUS_USERM_HANDLER_H_