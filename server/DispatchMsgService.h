#ifndef BRK_SERVICE_DISPATCH_EVENT_SERVIVE_H_
#define BRK_SERVICE_DISPATCH_EVENT_SERVIVE_H_

#include <map>
#include <vector>
#include "ievent.h"
#include "eventtype.h"
#include "iEventHandler.h"
#include "threadpool/thread_pool.h"
#include "bike.pb.h"
#include "NetworkInterface.h"
#include "events_def.h"
#include <queue>
#include "configdef.h"
#include "Logger.h"
#include "iniconfig.h"
class DispatchMsgService
{
protected:
    DispatchMsgService();

public:
    
    virtual ~DispatchMsgService();

    virtual BOOL open();
    virtual void close();

    virtual void subscribe(u32 eid, iEventHandler* handler);
    virtual void unsubscribe(u32 eid, iEventHandler* handler);
    virtual i32 enqueue(iEvent* ev);
    virtual iEvent* process(const iEvent* ev);//对具体的事件进行分发处理 
    static void svc(void* argv);//线程池回调函数
    static DispatchMsgService* getInstance();
    iEvent* parseEvent(const char* message, u32 len, u16 eid);

    void handleAllResponseEvent(NetworkInterface* interface);
    void package_message(iEvent * ev, i32 rsp_eid,NetworkInterface* interface);
protected:
    thread_pool_t* tp;

    static DispatchMsgService* DMS_;

    typedef std::vector<iEventHandler*> T_EventHandlers;
    typedef std::map<u32,T_EventHandlers> T_EventHandlersMap;
    T_EventHandlersMap subscribers_;

    bool svr_exit_;

    static std::queue<iEvent*> response_events;
    static pthread_mutex_t queue_mutex;

};

#endif