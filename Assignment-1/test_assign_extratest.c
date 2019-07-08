#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "storage_mgr.h"
#include "dberror.h"
#include "test_helper.h"

// test name
char *testName;

/* test output files */
#define TESTPF "test_pagefile_extra.bin"

/* prototypes for test functions */

static void testSinglePageExtra(void);

/* main function running all tests */
int
main (void)
{
  testName = "";

  initStorageManager();

  testSinglePageExtra();

  return 0;
}


/* Try to create, open, and close a page file */
void
testSinglePageExtra(void)
{
  SM_FileHandle fh;
  SM_PageHandle ph;
  int i;

  testName = "test single page content";

  ph = (SM_PageHandle) malloc(PAGE_SIZE);

  // create a new page file
  TEST_CHECK(createPageFile (TESTPF));
  TEST_CHECK(openPageFile (TESTPF, &fh));
  printf("created and opened file\n");

  // read first page into handle
  TEST_CHECK(readFirstBlock (&fh, ph));

  // the page should be empty (zero bytes)
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in first page of freshly initialized page");
  printf("first block was empty\n");

  // change ph to be a string and write that one to disk
  for (i=0; i < PAGE_SIZE; i++)
    ph[i] = (i % 10) + '0';
  TEST_CHECK(writeBlock (0, &fh, ph));
 printf("first block was empty\n");
  /*New test cases - Begins*/
  
  
  TEST_CHECK(appendEmptyBlock(&fh));
  TEST_CHECK(writeCurrentBlock(&fh, ph));
  TEST_CHECK(readBlock(0,&fh, ph));
  TEST_CHECK(readLastBlock(&fh, ph));
  TEST_CHECK(readCurrentBlock(&fh, ph));
  TEST_CHECK(ensureCapacity(100, &fh));
  /*New test cases - Ends*/

  //############additional part###############
  
    // Writing string ph to specific page of file.
	TEST_CHECK(writeBlock(2, &fh, ph));
	printf("write in third block \n");

	TEST_CHECK(writeBlock(3, &fh, ph))
	printf("write in fourth block \n");
  
    // Reading the previous block from file.
  	TEST_CHECK(readPreviousBlock (&fh, ph));
  	for (i=0; i < PAGE_SIZE; i++)
    	ASSERT_TRUE((ph[i] == (i % 10) + '0'), "readPreviousBlock check code");


    // Reading the next block from file.
  	TEST_CHECK(readNextBlock (&fh, ph));
  	for (i=0; i < PAGE_SIZE; i++)
    	ASSERT_TRUE((ph[i] == (i % 10) + '0'), "readNextBlock check code.");
  
  	// Reading the current block from file.
  	TEST_CHECK(readCurrentBlock (&fh, ph));
  	for (i=0; i < PAGE_SIZE; i++)
    	ASSERT_TRUE((ph[i] == (i % 10) + '0'), "readCurrentBlock check code");

  	// Reading the specific block  from file.	
  	TEST_CHECK(readBlock(2,&fh, ph));
	TEST_CHECK(readBlock(3,&fh, ph));
  	for (i=0; i < PAGE_SIZE; i++)
    	ASSERT_TRUE((ph[i] == (i % 10) + '0'), "reading the specific block checkcode.");

  	// Reading the last block from file.
  	TEST_CHECK(readLastBlock (&fh, ph));
  	for (i=0; i < PAGE_SIZE; i++)
    	ASSERT_TRUE((ph[i] == (i % 10) + '0'), "readLastBlock check code");
 
	
	
   //############additional part end###############
  printf("writing first block\n");

  // read back the page containing the string and check that it is correct
  TEST_CHECK(readFirstBlock (&fh, ph));
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
  printf("reading first block\n");

  // destroy new page file
  TEST_CHECK(destroyPageFile (TESTPF));

  TEST_DONE();

}
