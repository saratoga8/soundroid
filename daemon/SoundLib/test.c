#include "CUnit/Basic.h"
#include "SoundLib.h"
#include <stdlib.h>

#include "Log.h"

#define RESULT_LEN 10

char result[RESULT_LEN] = {'\0'};





int cleanSuite(void)
{
    return 0;
}

void execCommand(char *command)
{
    FILE *fp;
    int status;

    bzero(result, RESULT_LEN);
    
    fp = popen(command, "r");
    if (fp == NULL)
	{
	    printf("ERROR: Can't execute command\n");
	    return;
	}
    
    while (fgets(result, RESULT_LEN, fp) != NULL);
   
    status = pclose(fp);
    if (status == -1)
	{
	    printf("ERROR: Can't close the pipe of executing command\n");
	}
}

int initSuite(void)
{
    execCommand("amixer set Master 80%");
    return 0;
}

void testGetCurVol()
{
    execCommand("amixer sget Master | grep % | cut -d '[' -f 2 | sed 's/%.*//'");
    long curVol = atol(result);
    
    CU_ASSERT_EQUAL(curVol, getVol());
}

void testDecVol()
{
    execCommand("amixer sget Master | grep % | cut -d '[' -f 2 | sed 's/%.*//'");
    long prevVol = atol(result);

    #define DEC_VAL 11
    
    chgVol(-DEC_VAL);
    
    execCommand("amixer sget Master | grep % | cut -d '[' -f 2 | sed 's/%.*//'");
    long curVol = atol(result);
    
    CU_ASSERT_EQUAL(prevVol - DEC_VAL, curVol);
}

void testIncVol()
{
    execCommand("amixer sget Master | grep % | cut -d '[' -f 2 | sed 's/%.*//'");
    long prevVol = atol(result);

    #define DEC_VAL 11
    
    chgVol(DEC_VAL);
    
    execCommand("amixer sget Master | grep % | cut -d '[' -f 2 | sed 's/%.*//'");
    long curVol = atol(result);
    
    CU_ASSERT_EQUAL(prevVol + DEC_VAL, curVol);
}


void testMuteState()
{
    execCommand("amixer sget Master | grep % | cut -d '[' -f 4 | sed 's/].*//'");
    bool muted = (strcmp(result, "off\n") == 0);

    CU_ASSERT_TRUE(muted == isMuted());
}

void testSetMute()
{
    execCommand("amixer sget Master | grep % | cut -d '[' -f 4 | sed 's/].*//'");
    bool muted = (strcmp(result, "off\n") == 0);
    if(muted)
	{
	    unmute();
	    execCommand("amixer sget Master | grep % | cut -d '[' -f 4 | sed 's/].*//'");
	    CU_ASSERT_TRUE(strcmp(result, "on\n") == 0);
	    mute();
	    execCommand("amixer sget Master | grep % | cut -d '[' -f 4 | sed 's/].*//'");
	    CU_ASSERT_TRUE(strcmp(result, "off\n") == 0);	    
	}
    else
	{
	    mute();
	    execCommand("amixer sget Master | grep % | cut -d '[' -f 4 | sed 's/].*//'");
	    CU_ASSERT_TRUE(strcmp(result, "off\n") == 0);
	    unmute();
	    execCommand("amixer sget Master | grep % | cut -d '[' -f 4 | sed 's/].*//'");
	    CU_ASSERT_TRUE(strcmp(result, "on\n") == 0);
	}	
}

int main()
{
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   CU_pSuite pSuite = CU_add_suite("Suite1", initSuite, cleanSuite);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (NULL == CU_add_test(pSuite, "get current volume      ", testGetCurVol) ||
       NULL == CU_add_test(pSuite, "decrease volume         ", testDecVol)  ||
       NULL == CU_add_test(pSuite, "increase volume         ", testIncVol)  ||
       NULL == CU_add_test(pSuite, "check mute/unmute state ", testMuteState) ||
       NULL == CU_add_test(pSuite, "set mute/unmute state   ", testSetMute))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }
   
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();

   CU_cleanup_registry();
   return CU_get_error();
}


