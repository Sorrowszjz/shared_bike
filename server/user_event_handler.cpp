#include "user_event_handler.h"
#include "DispatchMsgService.h"
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>

UserEventHandler::UserEventHandler() : iEventHandler("UserEventHandler")
{
    //订阅事件的处理
    DispatchMsgService::getInstance()->subscribe(EEVENTID_GET_MOBLIE_CODE_REQ,(iEventHandler *)this);
    DispatchMsgService::getInstance()->subscribe(EEVENTID_LOGIN_REQ,(iEventHandler *)this);
    thread_mutex_create(&pm_);

}
UserEventHandler::~UserEventHandler()
{
    //退订事件的处理
    DispatchMsgService::getInstance()->unsubscribe(EEVENTID_GET_MOBLIE_CODE_REQ,this);
    thread_mutex_destroy(&pm_);

} 
iEvent* UserEventHandler::handle(const iEvent* ev)
{
    if(ev == NULL)
    {
        printf("arg ev is null.");
        return NULL;
    }
    u32 eid = ev->get_eid();
    if(eid == EEVENTID_GET_MOBLIE_CODE_REQ)
    {
        return handle_mobile_code_req((MobileCodeReqEv *)ev);
    }
    else if(eid == EEVENTID_LOGIN_REQ)
    {
        return handle_login_req((LoginReqEv *)ev);
    }
    else if(eid == EEVENTID_RECHARGE_REQ)
    {
        
    }
    else if(eid == EEVENTID_GET_ACCOUNT_BALANCE_REQ)
    {

    }
    else if(eid == EEVENTID_LIST_ACCOUNT_RECORDS_REQ)
    {

    }
    else if(eid == EEVENT_LIST_TRAVELS_REQ)
    {

    }
    return NULL;
}

MobileCodeRspEv* UserEventHandler::handle_mobile_code_req(MobileCodeReqEv *ev)
{
    // i32 icode = code_gen();
    // MobileCodeRspEv* rsp = new MobileCodeRspEv(ERRC_SUCCESS,icode);
    i32 icode = code_gen();
    LOG_DEBUG("UserEventHandler::handle_mobile_code_req generatoe icode : %d",icode);
    std::string mobile_ = ev->get_mobile();

    thread_mutex_lock(&pm_);
    m2c_[mobile_] = icode;
    thread_mutex_unlock(&pm_);
    
    return new MobileCodeRspEv(ERRC_SUCCESS,icode);
}
i32 UserEventHandler::code_gen()
{
    i32 code = 0;
    srand((unsigned int)time(NULL));
    code = (unsigned int)(rand() %1000000);
    return code;
}

LoginRspEv* UserEventHandler::handle_login_req(LoginReqEv* ev)
{
    LoginRspEv* res = nullptr;
    std::string mobile = ev->get_mobile();
    i32 code = ev->get_icode();
  
    thread_mutex_lock(&pm_);
    std::map<std::string,i32>::iterator iter = m2c_.find(mobile);

    if(iter == m2c_.end() || (iter != m2c_.end() && iter->second != code))
    {
        res = new LoginRspEv(ERRC_INVALID_DATA);
    }

    thread_mutex_unlock(&pm_);

    if(res) return res;

    std::shared_ptr<MysqlConnection> mysqlconn(new MysqlConnection);

    st_env_config conf_args = Iniconfig::getInstance()->getconfig();

    if(!mysqlconn->Init(conf_args.db_ip.c_str(),conf_args.db_port,conf_args.db_user.c_str(),
                        conf_args.db_pwd.c_str(),conf_args.db_name.c_str()))
    {
        LOG_ERROR("UserEventHandler::handle_login_req : Database init failed.exit");
        return new LoginRspEv(ERRO_PROCESS_FAILED);
    }
    //add new user
    UserService us(mysqlconn);
    bool result = false;

    if(!us.exist(mobile))
    {
        result = us.insert(mobile);
        if(!result)
        {
            LOG_ERROR("UserEventHandler::handle_login_req : insert user (%s) to db failed",mobile.c_str());
            return new LoginRspEv(ERRO_PROCESS_FAILED);
        }
    }
    return new LoginRspEv(ERRC_SUCCESS);
}