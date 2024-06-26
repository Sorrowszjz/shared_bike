#ifndef BUSSINESS_PROCESS__H_
#define BUSSINESS_PROCESS__H_
#include "user_event_handler.h"
#include "sqlconnection.h"
#include "SqlTables.h"
#include <memory>

class BusinessProcessor
{
public:
    BusinessProcessor(std::shared_ptr<MysqlConnection> conn);
    bool init();
    virtual ~BusinessProcessor();
private:
    std::shared_ptr<MysqlConnection> mysqlconn_;
    std::shared_ptr<UserEventHandler> ueh_;
};




#endif //BUSSINESS_PROCESS__H_