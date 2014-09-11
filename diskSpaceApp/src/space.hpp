#ifndef SPACE_HPP
#define SPACE_HPP 1

// Declare the function that computes % free disk space so it can be called from C

#ifdef __cplusplus
extern "C" {
#endif

#define P_PERCENT_FREE "PERCENT_FREE"
#define P_FREE_GB "FREE_GB"
#define P_SIZE_GB "SIZE_GB"

int getPercentFree(double *pFreeGb, double *pSizeGb, double *pPctFree);

#ifdef __cplusplus
}
#endif

#endif