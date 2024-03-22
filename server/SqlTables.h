#ifndef SQL_TABLES_H_
#define SQL_TABLES_H_

#include <memory>
#include "sqlconnection.h"
#include "glo_def.h"
class SqlTables
{
public:
    SqlTables(std::shared_ptr<MysqlConnection> sqlconn) : sqlconn_(sqlconn)
    {

    } 

    bool CreateUserInfo()
    {
        const char* pUserInfoTable = " \
                                    CREATE TABLE IF NOT EXISTS userinfo( \
                                        id          int(16)         NOT NULL primary key auto_increment, \
                                        mobile      varchar(16)     NOT NULL default '13000000000', \
                                        username    varchar(128)    NOT NULL default '', \
                                        verify      int(4)          NOT NULL default '0', \
                                        registertm  timestamp       NOT NULL default CURRENT_TIMESTAMP, \
                                        money       int(4)          NOT NULL default  0, \
                                        INDEX       mobile_index(mobile) \
                                    )";

        if(sqlconn_->Execute(pUserInfoTable))
        {
            LOG_ERROR("SqlTables::CreateUserInfo : create table failed.%s",sqlconn_->GetErrInfo());
            return 0;
        }
        return true;
    }

    bool CreateBikeTable()
    {
        const char* pBikeInfoTable = " \
                        CREATE TABLE IF NOT EXISTS bikeinfo( \
                            id         int          NOT NULL primary key auto_increment, \
                            devno      int          NOT NULL default 0, \
                            status     tinyint(1)   NOT NULL default 0, \
                            trouble    int          NOT NULL default 0, \
                            tmsg       varchar(256) NOT NULL default '', \
                            latitude   double(10,6) NOT NULL default 0, \
                            longitude  double(10,6) NOT NULL default 0, \
                            unique(devno) \
                        )";
        if(!sqlconn_->Execute(pBikeInfoTable))
        {
            LOG_ERROR("SqlTables::CreateBikeTable : create table bikeinfo failed.%s",sqlconn_->GetErrInfo());
            return 0;
        }
        return true;
    }
private:
    std::shared_ptr<MysqlConnection> sqlconn_;
};

#endif