#ifndef USER_SERVICE_H_
#define USER_SERVICE_H_

#include <memory>
#include "sqlconnection.h"
#include "SqlTables.h"

class UserService
{
public:
    UserService(std::shared_ptr<MysqlConnection> sql_conn);
    bool exist(const std::string&mobile);
    bool insert(const std::string& mobile);

private:
    std::shared_ptr<MysqlConnection> sql_conn_;

};

#endif //USER_SERVICE_H_