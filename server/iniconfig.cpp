#include "iniconfig.h"
#include <iniparser/iniparser.h>

Iniconfig* Iniconfig::config = nullptr;

Iniconfig::Iniconfig() : _isloaded(false)
{

}
Iniconfig::~Iniconfig()
{

}

Iniconfig* Iniconfig::getInstance()
{
    if(config == nullptr)
    {
        config = new Iniconfig();
    }
    return config;
}

bool Iniconfig::loadfile(const std::string& path)
{
    dictionary* ini = NULL;

    ini = iniparser_load(path.c_str());
    if(ini == NULL)
    {
        fprintf(stderr,"cannot parse file : %s\n",path.c_str());
        return false;
    }
    const char* ip = iniparser_getstring(ini,"database:ip","127.0.0.1");
    int   port  = iniparser_getint(ini, "database:port", 3306);
    const char* user  = iniparser_getstring(ini, "database:user", "root");
    const char* pwd   = iniparser_getstring(ini, "database:pwd", "123456");
    const char* db    = iniparser_getstring(ini, "database:db", "sharedbike");
    int   sport = iniparser_getint(ini, "server:port", 9090);
    int time_out_value = iniparser_getint(ini,"server:time_out_value",10);
    
    LOG_INFO("Iniconfig::loadfile : time_out_value : %d",time_out_value);
    _config = st_env_config(std::string(ip), port, std::string(user), 
    std::string(pwd), std::string(db), sport,time_out_value);

    iniparser_freedict(ini);

    _isloaded = true;

    return true;
}
const st_env_config& Iniconfig :: getconfig()
{
    return _config;
}