# shared_bike
* 编译指令：
* `g++ bike.pb.cpp ievent.cpp NetworkInterface.cpp  events_def.cpp eventtype.cpp test.cpp threadpool/*.cpp  user_event_handler.cpp DispatchMsgService.cpp iniconfig.cpp Logger.cpp  sqlconnection.cpp BusProcessor.cpp  -o test -std=c++11 -lprotobuf -levent -lpthread -liniparser -llog4cpp -I ../third/include/ -L ../third/lib/iniparser/ -L ../third/lib/log4cpp/ -lmysqlclient`
