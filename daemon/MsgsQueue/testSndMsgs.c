#include "CommandsNames.h"
#include "CUnit/Basic.h"
#include "MsgsQueueClient.h"
#include "unistd.h"

#include "Log.h"

#include "pthread.h"


pthread_t thread;

int initSuite(void)
{
    return 0;
}

int cleanSuite(void)
{
    return 0;
}

void testRunQueueClient()
{
    CU_ASSERT_TRUE(initQueueClient());
}

void testGetLocalIP()
{
    CU_ASSERT_TRUE(sendMsgClient(LOCAL_IP));
    char* ip = receiveMsgClient();
    CU_ASSERT_STRING_EQUAL(ip, "192.168.0.143");
}

void testGetConnectedIP()
{
    CU_ASSERT_TRUE(sendMsgClient(CONNECTED_IP));
    char* ip = receiveMsgClient();
    CU_ASSERT_STRING_EQUAL(ip, "None");
}

void testGetCurrentPort()
{
    CU_ASSERT_TRUE(sendMsgClient(GET_PORT));
    char* ip = receiveMsgClient();
    CU_ASSERT_STRING_EQUAL(ip, "5000");
}

int main()
{
   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   CU_pSuite pSuite = CU_add_suite("Suite1", initSuite, cleanSuite);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (NULL == CU_add_test(pSuite, "init client queue ", testRunQueueClient) ||
       NULL == CU_add_test(pSuite, "get local IP      ", testGetLocalIP)     ||
       NULL == CU_add_test(pSuite, "get connected IP  ", testGetConnectedIP) ||
       NULL == CU_add_test(pSuite, "get current port  ", testGetCurrentPort))
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
