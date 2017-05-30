#include "CUnit/Basic.h"
#include "../file_utils.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include "../proc_utils.h"

#define FILE_NAME "file.txt"
#define TXT "string from file"
#define TXT_LEN 17

char *expectedParams;

int initSuite(void)
{
    FILE *file = fopen(FILE_NAME, "w+");
    if (file == NULL)
	{
	    printf("ERROR: Can't open file %s\n", FILE_NAME);
	    return 1;
	}
    fprintf(file, "%s\n", TXT);
    fclose(file);
    
    return 0;
}

int cleanSuite(void)
{
    return 0;
}

void testGetStr()
{
    char *txt = (char*) calloc(sizeof(char), TXT_LEN);
    CU_ASSERT_TRUE(cpStrFromFileToBuff(FILE_NAME, TXT_LEN, txt));
    CU_ASSERT_STRING_EQUAL(txt, TXT);
    free(txt);
}

void testFileInDir()
{
    CU_ASSERT_TRUE(doesDirHasFile(".", FILE_NAME));
}

void testDelFileByPath()
{
    delFileByPath(FILE_NAME);
    CU_ASSERT_EQUAL(access(FILE_NAME, F_OK), -1);
}

void testRunFile()
{
    CU_ASSERT_TRUE(runFileInCurDir(2, (const char* []){"./runable", FILE_NAME}));
    
    char *txt = (char*) calloc(sizeof(char), TXT_LEN);
    CU_ASSERT_TRUE(cpStrFromFileToBuff(FILE_NAME, TXT_LEN, txt));
    CU_ASSERT_STRING_EQUAL(txt, TXT);
    free(txt);    
}

void testGetProcParams()
{
    pid_t PID = getpid();
    char pidStr[10] = {0};
    sprintf(pidStr, "%d", PID);
    char *params = getProcessParams(pidStr);
    
    CU_ASSERT_STRING_EQUAL(params, expectedParams);

    free(params);
    free(expectedParams);
}

int main(int argc, char* argv[])
{
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   CU_pSuite pSuite = CU_add_suite("Suite1", initSuite, cleanSuite);
   if (NULL == pSuite)
       {
	   CU_cleanup_registry();
	   return CU_get_error();
       }

   expectedParams = (char*) calloc (sizeof(char), 1024);
   int i;
   for (i = 0; i < argc; ++i)
       {
	   expectedParams = strcat(expectedParams, argv[i]);
	   expectedParams = strcat(expectedParams, " ");
       }
   
   if (NULL == CU_add_test(pSuite, "get string from file ", testGetStr)    ||
       NULL == CU_add_test(pSuite, "current dir has file ", testFileInDir) ||
       NULL == CU_add_test(pSuite, "run file             ", testRunFile)   ||
       NULL == CU_add_test(pSuite, "delete file by path  ", testDelFileByPath) ||
       NULL == CU_add_test(pSuite, "get proc's parameters", testGetProcParams))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();

   CU_cleanup_registry();
   return CU_get_error();
}
