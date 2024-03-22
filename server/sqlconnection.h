#ifndef DATABASE_SQLCONN_H_
#define DATABASE_SQLCONN_H_

#include <mysql/mysql.h>
#include <string>
#include <mysql/errmsg.h>
#include <assert.h>
#include <string.h>

#include "glo_def.h"

class SqlRecordSet
{
public:
    SqlRecordSet() : m_pRes(NULL)
    {

    }

    explicit SqlRecordSet(MYSQL_RES* pRes) { m_pRes = pRes; }

    MYSQL_RES* MysqlRes()
    {
        return m_pRes;
    }
    
    ~SqlRecordSet()
    {
        if(m_pRes)
        {
            mysql_free_result(m_pRes);
        }
    }
    
    inline void SetResult(MYSQL_RES* pRes)
    {
        assert(m_pRes == NULL);
        if(m_pRes)
        {
            LOG_WARN("SqlRecordSet::SetResult : the MYSQL_RES has already stored, may cause memory leak");
        }
        m_pRes = pRes;
    }
    
    inline MYSQL_RES* GetResult()
    {
        return m_pRes;
    }
    
    void FetchRow(MYSQL_ROW& row)
    {
        row = mysql_fetch_row(m_pRes);
    }
    
    inline i32 GetRowCount()
    {
        return m_pRes->row_count;
    }
private:
    MYSQL_RES* m_pRes;
};

class MysqlConnection
{
public:
    MysqlConnection();
    ~MysqlConnection();

    MYSQL* mysql() {return mysql_;}

    bool Init(const char* szHost, int nPort, const char* szUser, const char* szPasswd, const char* szDb);

    bool Execute(const char* szSql);

    bool Execute(const char* szSql, SqlRecordSet& recordset);

    void Close();

    const char* GetErrInfo();

    void Reconnect();

    int EscapeString(const char* pSrc, int nSrcLen, char* pDest);

private:
    MYSQL* mysql_;

};


#endif //DATABASE_SQLCONN_H_