#include <windows.h>
#include <iostream>
#include <stdlib.h>

#include "space.hpp"

static char *drive = NULL;

// Return the % free space
int getPercentFree(double *pFreeGb, double *pSizeGb, double *pPctFree) {
	ULARGE_INTEGER ulFree;
	ULARGE_INTEGER ulSize;
	
	if ( drive==NULL ) {
		drive = getenv("PATH_TO_CHECK");
	}
	
	if ( drive==NULL ) {
		std::cerr << "Error: PATH_TO_CHECK not set" << std::endl;
		*pFreeGb = -1.0;
		*pSizeGb = -1.0;
		*pPctFree = -1.0;
		return 0;
	}
	else {
		double bytesToGb = 1.0 / 1024.0 / 1024.0 / 1024.0;
		BOOL ret = GetDiskFreeSpaceEx(drive, &ulFree, &ulSize, NULL);
		//printf("%llu %llu\n", ulFree, ulSize);
		*pFreeGb = bytesToGb * (double)ulFree.QuadPart;
		*pSizeGb = bytesToGb * (double)ulSize.QuadPart;
		*pPctFree = 100.0 * *pFreeGb / *pSizeGb;
	
		return 1;
	}
}

