#include "CUnit/Basic.h"
#include "MsgsQueueServer.h"
#include "MsgsQueueClient.h"
#include "unistd.h"
#include "Log.h"

#include "pthread.h"


pthread_t thread;

int initSuite(void)
{
    remove(FILE_NAME); 
    return 0;
}

int cleanSuite(void)
{
    remove(FILE_NAME); 
    return 0;
}

void testInitQueueServer()
{
    CU_ASSERT_TRUE(initQueueServer());
    CU_ASSERT_EQUAL(access(FILE_NAME, F_OK), 0);
}

void testDelQueueServer()
{
    deleteQueue();

    pthread_join(thread, NULL);
    //    pthread_exit(NULL);
    
    CU_ASSERT_NOT_EQUAL(access(FILE_NAME, F_OK), 0);
}

void *runServerQueue()
{
    runQueue();
    return NULL;
}

void testRunQueueServer()
{
    if(pthread_create(&thread, NULL, runServerQueue, NULL) != 0)
	{
	    CU_FAIL("Can't create thread for Server Queue");
	    return;
	}
    
    CU_ASSERT_NOT_EQUAL(access(LOG_FILE_NAME, F_OK), 0);
}

void testRunQueueClient()
{
    CU_ASSERT_TRUE(initQueueClient());
    CU_ASSERT_NOT_EQUAL(access(LOG_FILE_NAME, F_OK), 0);
}

void testSendMsgFromClient()
{
    CU_ASSERT_TRUE(sendMsgClient("client"));
    CU_ASSERT_NOT_EQUAL(access(LOG_FILE_NAME, F_OK), 0);
}

void testReceiveMsgByServer()
{
    sleep(2);
    CU_ASSERT_STRING_EQUAL(receiveMsgServer(), "client");
    CU_ASSERT_NOT_EQUAL(access(LOG_FILE_NAME, F_OK), 0);
}

void testSendMsgFromServer()
{
    CU_ASSERT_TRUE(sendMsgServer("server"));
    CU_ASSERT_NOT_EQUAL(access(LOG_FILE_NAME, F_OK), 0);
}

void testReceiveMsgByClient()
{
    CU_ASSERT_STRING_EQUAL(receiveMsgClient(), "server");
    CU_ASSERT_NOT_EQUAL(access(LOG_FILE_NAME, F_OK), 0);
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

   if (NULL == CU_add_test(pSuite, "init server queue    ", testInitQueueServer)    ||
       NULL == CU_add_test(pSuite, "init client queue    ", testRunQueueClient)     ||
       NULL == CU_add_test(pSuite, "send msg from server ", testSendMsgFromServer)  ||
       NULL == CU_add_test(pSuite, "receive msg by client", testReceiveMsgByClient) ||
       NULL == CU_add_test(pSuite, "run server queue     ", testRunQueueServer)     ||
       NULL == CU_add_test(pSuite, "send msg from client ", testSendMsgFromClient)  ||
       NULL == CU_add_test(pSuite, "receive msg by server", testReceiveMsgByServer) ||
       NULL == CU_add_test(pSuite, "delete server queue  ", testDelQueueServer) )
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
