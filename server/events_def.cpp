#include "events_def.h"
#include <string>
#include <iostream> 

std::ostream& MobileCodeReqEv::dump(std::ostream& out) const
{
    out << "Mobile : " << msg_.mobile()
    << "," << "Sn : " << get_sn() << std::endl;
    return out;
}

std::ostream& MobileCodeRspEv::dump(std::ostream& out) const
{
    out << "Code : " << msg_.code() << " , icode : " << msg_.icode()
    <<" , desc : " << msg_.data() << " ,sn : " << get_sn() <<std::endl;
    return out;
}

std::ostream& LoginReqEv::dump(std::ostream& out) const
{
    out << "mobile : " << msg_.mobile() << " ,icode : " << msg_.icode() << " sn : "
    << get_sn() << std::endl;
    return out;
}

std::ostream& LoginRspEv::dump(std::ostream& out) const
{
    out << "code : " << msg_.code() << " ,desc : " << msg_.desc() << " sn : "
    << get_sn() << std::endl;
    return out;
}