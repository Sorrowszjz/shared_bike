#include "ievent.h"

iEvent::iEvent(u32 eid, u32 sn) : eid_(eid) , sn_(sn)
{

}

iEvent::~iEvent(){}

u32 iEvent::generateSeqNo()
{
    static u32 sq = 0;
    return sq++;
}