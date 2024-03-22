#include "DispatchMsgService.h"
#include <algorithm>


DispatchMsgService* DispatchMsgService::DMS_ = nullptr;
pthread_mutex_t DispatchMsgService::queue_mutex;
std::queue<iEvent*> DispatchMsgService::response_events;
DispatchMsgService::DispatchMsgService() : tp(NULL)
{

}
DispatchMsgService::~DispatchMsgService()
{

}
BOOL DispatchMsgService::open()
{
    svr_exit_ = false;
    thread_mutex_create(&queue_mutex);
    tp = thread_pool_init();
    return tp ? true : false;
}

void DispatchMsgService::close()
{
    svr_exit_ = false;

    thread_pool_destroy(tp);
    thread_mutex_destroy(&queue_mutex);
    subscribers_.clear();

    tp = NULL;
}

void DispatchMsgService::svc(void *argv)
{
    DispatchMsgService* dms = DispatchMsgService::getInstance();
    iEvent* ev = (iEvent *)argv;
    if(!dms->svr_exit_)
    {
        LOG_DEBUG("DispatchMsgService::svc ... ");
        iEvent* rsp = dms->process(ev);
        /* ... */
        if(rsp)
        {
            rsp->dump(std::cout);
            rsp->set_args(ev->get_args());
        }
        else
        {
            rsp = new ExitRspEv();
            rsp->set_args(ev->get_args());
        }
        thread_mutex_lock(&queue_mutex);
        response_events.push(rsp);
        thread_mutex_unlock(&queue_mutex);
        //delete ev;
    }
}
DispatchMsgService* DispatchMsgService::getInstance()
{
    if(DMS_ == NULL)
    {
        DMS_ = new DispatchMsgService();
    }
    //printf("DMS : %p\n",DMS_);
    return DMS_;
}

i32 DispatchMsgService::enqueue(iEvent* ev)
{
    if(ev == NULL)
    {
        return -1;
    }

    thread_task_t* task = thread_task_alloc(0);

    task->handler = DispatchMsgService::svc;
    task->ctx = ev;

    return thread_task_post(tp,task);

}
void DispatchMsgService::subscribe(u32 eid,iEventHandler* handler)
{
    LOG_DEBUG("DispatchMsgService::subscribe eid : %u",eid);
    T_EventHandlersMap::iterator iter = subscribers_.find(eid);
    if(iter != subscribers_.end())
    {
        T_EventHandlers::iterator handler_iter = std::find(iter->second.begin(),iter->second.end(),handler);
        if(handler_iter == iter->second.end())
        {
            iter->second.push_back(handler);
        }
        //已经产生了相同订阅就啥也不干
    }
    else
    {
        subscribers_[eid].push_back(handler);
        //printf("%p\n",*subscribers_[eid].begin());
    }
}
void DispatchMsgService::unsubscribe(u32 eid,iEventHandler* handler)
{
    LOG_DEBUG("DispatchMsgService::unsubscribe eid : %u",eid);
    T_EventHandlersMap::iterator iter = subscribers_.find(eid);
    if(iter != subscribers_.end())
    {
        T_EventHandlers::iterator handler_iter = std::find(iter->second.begin(),iter->second.end(),handler);
        if(handler_iter != iter->second.end())
        {
            iter->second.erase(handler_iter);
            LOG_DEBUG("DispatchMsgService::unsubscribe eid : %u",eid);
        }
    }
}
iEvent* DispatchMsgService::process(const iEvent* ev)
{
    LOG_DEBUG("DispatchMsgService::process -ev: %p",ev);
    if(NULL == ev) return nullptr;
    
    u32 eid = ev->get_eid();

    LOG_DEBUG("DispatchMsgService::process -eid: %u",eid);

    if(eid == EEVENTID_UNKOWN)
    {
        LOG_WARN("DispatchMsgService::process unknown event id %d", eid);
        return nullptr;
    }

    T_EventHandlersMap::iterator iter = subscribers_.find(eid);
    if(iter == subscribers_.end())
    {
        LOG_WARN("DispatchMsgService::process  no suitiable handler for current event : %u",eid);
        return nullptr;
    }
    iEvent* rsp = nullptr;
    for(auto hdl = iter->second.begin() ; hdl != iter->second.end(); hdl++)
    {
        iEventHandler* handler = *hdl;
        //LOG_WARN("load handler : %s\n",hdl->get_name());
        rsp = handler->handle(ev);
    }
    return rsp;
}

iEvent* DispatchMsgService::parseEvent(const char* message, u32 len, u16 eid)
{
    if(!message)
    {
        LOG_ERROR("DispatchMsgService::parseEvent : message is null [eid : %d].",eid);
        return nullptr;
    }
    if(eid == EEVENTID_GET_MOBLIE_CODE_REQ)
    {
        tutorial::mobile_request mr;
        if(mr.ParseFromArray(message,len))
        {
            MobileCodeReqEv* ev = new MobileCodeReqEv(mr.mobile());
            return ev;
        }
    }
    else if(eid == EEVENTID_LOGIN_REQ)
    {
        tutorial::login_request lr;
        if(lr.ParseFromArray(message,len))
        {
            LoginReqEv* ev = new LoginReqEv(lr.mobile(),lr.icode());
            return ev;
        }
    }
    return nullptr;
}

void DispatchMsgService::handleAllResponseEvent(NetworkInterface* interface)
{
    bool done = 0;
    while(!done)
    {
        iEvent* ev = nullptr;
        thread_mutex_lock(&queue_mutex);
        if(!response_events.empty())
        {
            ev = response_events.front();
            response_events.pop();
        }  
        else 
        {
            done = 1;
        }
        thread_mutex_unlock(&queue_mutex);
        if(!done)
        {
            if(ev->get_eid() != EEVENT_EXIT_RSP)
            {
                //MobileCodeRspEv* mcre = static_cast<MobileCodeRspEv *>(ev);
                /*LOG_DEBUG("DispatchMsgService::handleAllResponseEvent : eid : %u",ev->get_eid());
                ConnectSession* cs = (ConnectSession *)ev->get_args();
                cs->response = ev;

                cs->message_len = ev->ByteSize();
                cs->write_buf = new char[cs->message_len + MESSAGE_HEADER_LEN];

                memcpy(cs->write_buf,MESSAGE_HEADER_ID,4);
                *(u16*)(cs->write_buf + 4) = EEVENTID_GET_MOBLIE_CODE_RSP;
                *(i32*)(cs->write_buf + 6) = cs->message_len;

                ev->SerializeToArray(cs->write_buf + MESSAGE_HEADER_LEN , cs->message_len);

                interface->send_response_message(cs);*/

                package_message(ev,ev->get_eid(),interface);
            }
            else
            {
                ConnectSession* cs = (ConnectSession *)ev->get_args();
                cs->response = ev;
                interface->send_response_message(cs);
            }
        }

    }
}

void DispatchMsgService::package_message(iEvent* ev, i32 rsp_eid,NetworkInterface* interface)
{
    //MobileCodeRspEv* mcre = static_cast<MobileCodeRspEv *>(ev);
    LOG_DEBUG("DispatchMsgService::handleAllResponseEvent : eid : %u",ev->get_eid());
    ConnectSession* cs = (ConnectSession *)ev->get_args();
    cs->response = ev;

    cs->message_len = ev->ByteSize();
    cs->write_buf = new char[cs->message_len + MESSAGE_HEADER_LEN];

    memcpy(cs->write_buf,MESSAGE_HEADER_ID,4);
    *(u16*)(cs->write_buf + 4) = rsp_eid;
    *(i32*)(cs->write_buf + 6) = cs->message_len;

    ev->SerializeToArray(cs->write_buf + MESSAGE_HEADER_LEN , cs->message_len);

    interface->send_response_message(cs);
}
