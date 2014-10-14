#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <epicsThread.h>
#include <iocsh.h>
#include <epicsExport.h>
#include "space.hpp"

CFreeSpace::CFreeSpace(const char* portName, const char* diskName) : asynPortDriver(portName, 
                    1, /* maxAddr */ 
                    NUM_MSP_PARAMS, /* num parameters */
                    asynFloat64Mask | asynDrvUserMask, /* Interface mask */
                    asynFloat64Mask,  /* Interrupt mask */
                    ASYN_CANBLOCK, /* asynFlags.  This driver can block but it is not multi-device */
                    1, /* Autoconnect */
                    0,
                    0), mDiskName(diskName) {
   	createParam(P_FreePctString, asynParamFloat64, &P_FreePct);
   	createParam(P_FreeGBString, asynParamFloat64, &P_FreeGB);
   	createParam(P_SizeGBString, asynParamFloat64, &P_SizeGB);
	createParam(P_UpdateTimeString, asynParamFloat64, &P_UpdateTime);
    setDoubleParam(P_UpdateTime, 10.0);

    eventId = epicsEventCreate(epicsEventEmpty);
	
	// Start the thread to check disk space
	asynStatus status = (asynStatus)(epicsThreadCreate("DiskSpaceTask",
                          epicsThreadPriorityMedium,
                          epicsThreadGetStackSize(epicsThreadStackMedium),
                          (EPICSTHREADFUNC)::simTask,
                          this) == NULL);
    if (status) {
        printf("%s: epicsThreadCreate failure\n",  diskName);
        return;
    }	
}

asynStatus CFreeSpace::writeFloat64(asynUser *pasynUser, epicsFloat64 value) {
    int function = pasynUser->reason;
	asynStatus status = (asynStatus) setDoubleParam(function, value);
	if (!status) {
		status = (asynStatus) callParamCallbacks();
	}
	return status;
}

void CFreeSpace::poll() {
	double updateTime;
	double bytesToGb = 1.0 / 1024.0 / 1024.0 / 1024.0;
	ULARGE_INTEGER ulFree;
	ULARGE_INTEGER ulSize;
	
	while ( 1 ) {
		BOOL ret = GetDiskFreeSpaceEx(mDiskName.c_str(), &ulFree, &ulSize, NULL);
		//printf("Ding: %f %llu\n", bytesToGb * (double)ulFree.QuadPart, ulSize);
		setDoubleParam(P_FreeGB, bytesToGb * (double)ulFree.QuadPart);
		setDoubleParam(P_SizeGB, bytesToGb * (double)ulSize.QuadPart);
		setDoubleParam(P_FreePct, 100.0 * (double)ulFree.QuadPart / (double)ulSize.QuadPart);
		callParamCallbacks();
		
		getDoubleParam(P_UpdateTime, &updateTime);
		//printf("Ding %f\n", updateTime);
		epicsEventWaitWithTimeout(this->eventId, updateTime);
	}
}

// The thread
void simTask(void *drvPvt)
{
    CFreeSpace *pPvt = (CFreeSpace *)drvPvt;
    
    pPvt->poll();
}


extern "C" {

// Initialise
int diskSpaceConfigure(const char *portName, const char* diskName)
{
	try
	{
		new CFreeSpace(portName, diskName);
		return(asynSuccess);
	}
	catch(const std::exception& ex)
	{
		std::cerr << "diskSpace failed: " << ex.what() << std::endl;
		return(asynError);
	}
}

// EPICS iocsh shell commands 

static const iocshArg initArg0 = { "portName", iocshArgString};			///< The name of the asyn driver port we will create
static const iocshArg initArg1 = { "diskName", iocshArgString};			///< The name of the lookup file

static const iocshArg * const initArgs[] = { &initArg0, &initArg1 };

static const iocshFuncDef initFuncDef = {"diskSpaceConfigure", sizeof(initArgs) / sizeof(iocshArg*), initArgs};

static void initCallFunc(const iocshArgBuf *args)
{
    diskSpaceConfigure(args[0].sval, args[1].sval);
}

static void diskSpaceRegister(void)
{
    iocshRegister(&initFuncDef, initCallFunc);
}

epicsExportRegistrar(diskSpaceRegister);

}

