#ifndef SPACE_HPP
#define SPACE_HPP 1

// Declare the function that computes % free disk space so it can be called from C

//#ifdef __cplusplus
#include <string>
#include "asynPortDriver.h"

//extern "C" {
//#endif
//
//#define P_PERCENT_FREE "PERCENT_FREE"
//#define P_FREE_GB "FREE_GB"
//#define P_SIZE_GB "SIZE_GB"
//
//#ifdef __cplusplus
//}

void simTask(void *drvPvt);

class CFreeSpace : public asynPortDriver 
{
public:
    CFreeSpace(const char* portName, const char* diskName);
	virtual ~CFreeSpace() {}
	void poll();
	asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
	
private:
	std::string mDiskName;
	epicsEventId eventId;
	
	int P_FreePct;
	int P_FreeGB;
	int P_SizeGB;
	int P_UpdateTime;
#define FIRST_MSP_PARAM P_FreePct
#define LAST_MSP_PARAM P_UpdateTime    
};

#define NUM_MSP_PARAMS (&LAST_MSP_PARAM - &FIRST_MSP_PARAM + 1)

#define P_FreePctString		"PERCENT_FREE"
#define P_FreeGBString		"FREE_GB"
#define P_SizeGBString		"SIZE_GB"
#define P_UpdateTimeString	"UPDATE_TIME"
//#ifdef __cplusplus
//#endif

#endif