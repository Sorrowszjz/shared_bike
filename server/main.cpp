#include <iostream>
#include "ievent.h"
#include "bike.pb.h"
#include "events_def.h"
#include "glo_def.h"
#include "eventtype.h"
#include "user_event_handler.h"
#include "DispatchMsgService.h"
#include <unistd.h>
#include "NetworkInterface.h"
#include "configdef.h"
#include "Logger.h"
#include "iniconfig.h"
#include "sqlconnection.h"
#include "SqlTables.h"
#include "BusProcessor.h"
using namespace tutorial;
int main(int argc, char** argv)
{

    if(argc != 3)
    {
        printf("Please input shbk <config file path> <log file config> !\n");
        return -1;
    }

    if(!Logger::instance()->init(std::string(argv[2])))
    {
        fprintf(stderr,"init log module failed.\n");
        return -2;
    }

    Iniconfig* config = Iniconfig::getInstance();
    if(!config->loadfile(std::string(argv[1])))
    {
        //printf("load %s failed.\n",argv[1]);
        LOG_ERROR("load %s failed.",argv[1]);
        return -3;
    }

    st_env_config  conf_args = config->getconfig();

    LOG_INFO("[database] ip : %s port : %d user : %s pwd : %s db : %s [server] port : %d time_out_val : %d",conf_args.db_ip.c_str(),conf_args.db_port,
    conf_args.db_user.c_str(),conf_args.db_pwd.c_str(),conf_args.db_name.c_str(),conf_args.svr_port,conf_args.time_out_value);


    std::shared_ptr<MysqlConnection> mysqlconn(new MysqlConnection);
    if(!mysqlconn->Init(conf_args.db_ip.c_str(),conf_args.db_port,conf_args.db_user.c_str(),
                        conf_args.db_pwd.c_str(),conf_args.db_name.c_str()))
    {
        LOG_ERROR("MAIN::Database init failed.exit");
        return -4;
    }

    BusinessProcessor busPro(mysqlconn);
    busPro.init();
    UserEventHandler* uel = new UserEventHandler();
    DispatchMsgService* DMS = DispatchMsgService::getInstance();
    DMS->open();

    NetworkInterface* NTIF = new NetworkInterface();
    NTIF->start(conf_args.svr_port);
    while(true)
    {
        NTIF->net_work_event_dispatch();
        sleep(1);
        //LOG_DEBUG("network_event_dispatch ... \n");
    }


    
    // MobileCodeReqEv* me = new MobileCodeReqEv("15023374462");
    
    //DMS->enqueue(me);
    //sleep(5);
    DMS->close();

    sleep(5);

    return 0;
}