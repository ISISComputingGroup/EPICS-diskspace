#include <stdlib.h>
#include <stdio.h>
#include <dbAccess.h>
#include <devSup.h>
#include <recGbl.h>

#include <aiRecord.h>

#include <epicsExport.h>

#include "space.hpp"

static long read_ai(aiRecord *pao);

struct {
  long num;
  DEVSUPFUN  report;
  DEVSUPFUN  init;
  DEVSUPFUN  init_record;
  DEVSUPFUN  get_ioint_info;
  DEVSUPFUN  read_ai;
  DEVSUPFUN  special_linconv;
} devAiFree = {
  6, /* space for 6 functions */
  NULL,
  NULL,
  NULL,
  NULL,
  read_ai,
  NULL
};
epicsExportAddress(dset,devAiFree);

static int endsWith(char *ending, char *target) {
	int eLen = strlen(ending);
	int tLen = strlen(target);
	if ( eLen>tLen ) {
		return 0;
	}
	target += tLen - eLen;
	return strcmp(ending, target)==0;
}

// Set the percent free
static long read_ai(aiRecord *pao)
{
	double freeGb;
	double sizeGb;
	double pctFree;
	
	getPercentFree(&freeGb, &sizeGb, &pctFree);
	
	if ( endsWith(P_PERCENT_FREE, pao->name) ) 
	{
		pao->val = pctFree;
	}
	else if ( endsWith(P_FREE_GB, pao->name) ) 
	{
		pao->val = freeGb;
	}
	else if ( endsWith(P_SIZE_GB, pao->name) ) 
	{
		pao->val = sizeGb;
	}
	else 
	{
		pao->val = -1.0;
	}

  // Do not convert
  return 2;
}
