#include "CUnit/Basic.h"
#include "../Log.h"
#include "errno.h"
#include "string.h"
#include "unistd.h"
#include "stdlib.h"

#define LOG_FILE_NAME "log.txt"

FILE *file = NULL;

#define BUFF_LEN 100
char buff[BUFF_LEN] = {'\0'};

char *date;

int initSuite(void)
{
    remove(LOG_FILE_NAME); 
    return 0;
}

int cleanSuite(void)
{
    if(fclose(file) == EOF)
	{
	    return -1;
	}
    return 0;
}


void testOpenLogFile()
{
    openLogFile(LOG_FILE_NAME);
    closeLogFile(LOG_FILE_NAME);
    file = fopen(LOG_FILE_NAME, "r");
    if(file != NULL)
	{
	    CU_ASSERT(0 == fread(buff, sizeof(char), BUFF_LEN, file));
	}
    else
	{
	    CU_FAIL("Can't open log file");
	}
}

void testWrite2Log()
{
    if(file != NULL)
	{
	    writeToLog("Test1\n", "TAG");
	    CU_ASSERT(0 != fread(buff, sizeof(char), BUFF_LEN, file));
	    CU_ASSERT_PTR_NOT_NULL(strstr(buff, "TAG: Test1"));
	}
    else
	{
	    CU_FAIL("Can't open log file");
	}
}

void testAddDateToTxt()
{
    if(file != NULL)
	{
	    char *resTxt = addDateToTxt("Text", "TAG");
	    CU_ASSERT_PTR_NOT_NULL(strstr(resTxt, date));
	    CU_ASSERT_PTR_NOT_NULL(strstr(resTxt, "Text"));
	    CU_ASSERT_PTR_NOT_NULL(strstr(resTxt, "TAG"));
	    free(resTxt);
	}
    else
	{
	    CU_FAIL("Can't open log file");
	}    
}

void testWrite2Log2()
{
    if(file != NULL)
	{
	    writeToLog2("Test2", "Test3\n", "TAG");
	    CU_ASSERT(0 != fread(buff, sizeof(char), BUFF_LEN, file));
	    CU_ASSERT_PTR_NOT_NULL(strstr(buff, "TAG: Test2Test3"));
	}
    else
	{
	    CU_FAIL("Can't open log file");
	}
}

void testWriteToLogIfError()
{
    if(file != NULL)
	{
	    writeToLogIfError(-1, __func__, "error", "TAG");
	    CU_ASSERT(0 != fread(buff, sizeof(char), BUFF_LEN, file));
	    CU_ASSERT_PTR_NOT_NULL(strstr(buff, "TAG: ERROR: testWriteToLogIfError(): error"));
	}
    else
	{
	    CU_FAIL("Can't open log file");
	}    
}

void testClrLog()
{
    int i;
    for(i = 0; getLogSizeBytes() < MAX_LOG_FILE_LEN; ++i)
	{
	    writeToLog2("test2", "test3", "tag");
	}
    writeToLog2("test2", "test3", "tag");
    CU_ASSERT(getLogSizeBytes() < MAX_LOG_FILE_LEN);
}


int main(const int argc, char* argv[])
{
    if(argc == 1)
	{
	    printf("ERROR: the current date should be given as parameter!\n");
	    return -1;
	}
    
    date = argv[1];
   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   CU_pSuite pSuite = CU_add_suite("Suite1", initSuite, cleanSuite);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (NULL == CU_add_test(pSuite, "open log's file              ", testOpenLogFile)       ||
       NULL == CU_add_test(pSuite, "write to log's file          ", testWrite2Log)         ||
       NULL == CU_add_test(pSuite, "write 2 strings to log's file", testWrite2Log2)        ||
       NULL == CU_add_test(pSuite, "add date to a text           ", testAddDateToTxt)      ||
       NULL == CU_add_test(pSuite, "write to log if error        ", testWriteToLogIfError) ||
       NULL == CU_add_test(pSuite, "clearing log's file          ", testClrLog))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }
   
   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();

   /* Clean up registry and return */
   CU_cleanup_registry();
   return CU_get_error();
}

