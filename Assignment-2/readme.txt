
(1) Files list: 
----------------------------------------------------------
        README.txt
        dberror.c
        dberror.h
	buffer_mgr.c
	buffer_mgr.h
	buffer_mgr_stat.c
	buffer_mgr_stat.h
        storage_mgr.c
        storage_mgr.h
        test_assign2_1.c
	test_assign2_2.c
        test_helper.h
	dt.h
	Makefile
	pthreads

----------------------------------------------------------

(2) How to use the Makefile:
----------------------------------------------------------
On linux system,

1.Open the terminal and cd to the address of the "assign1" where you put.

2.Type in "make clean" and click "enter". (make sure no other files except as the above files in the assign1)
If shows as:
"rm -f test1 test2 *.o *~"
Continue step 3.   Else: Check if address is correct.

3a.Type in "make", Click "enter".(run test case in test_assign2_1, if you would like to run test_assign2_2 please see step 3b)
3b. (1) amend default: test1 to default: test2
    (2) comment out following codes:
	test1: test_assign2_1.o storage_mgr.o dberror.o buffer_mgr.o buffer_mgr_stat.o
	$(CC) $(CFLAGS) -o test1 test_assign2_1.o storage_mgr.o dberror.o buffer_mgr.o 		buffer_mgr_stat.o -lm

	test_assign2_1.o: test_assign2_1.c dberror.h storage_mgr.h test_helper.h 		buffer_mgr.h buffer_mgr_stat.h
	$(CC) $(CFLAGS) -c test_assign2_1.c -lm
	
	run_test1:
	./test1
    (3) repeat step 1 - 3
	
    (4) Type in "make run_test2" , Click "enter".(Output the testing result)

4.Type in "make run_test1" , Click "enter".(Output the testing result)


--------------------
On Windows system
pthreads contains header files/libs for win32 support.
Need include those in your VS IDE.


(3) Data Structures & Main idea
---------------------------------
typedef unsigned long long BigInt; 
//ulonglong

//setup our global mutex virable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct BM_BufferPool {
	char *pageFile;
	int numPages;
	ReplacementStrategy strategy;
	void *mgmtData; // use this one to store the bookkeeping info your buffer 
					// manager needs for a buffer pool
	void *stratData;// strategy data, used for thread safe.
} BM_BufferPool;

//original data type is not strong enough to use thus I extend it to


#include <time.h>

typedef struct BM_PageHandle {
	PageNumber pageNum;
	char *data;

	int fixcount;	//reference count by clients
	int NumReadIO;  //the number of pages that have been read from disk since a buffer pool has been initialized
	int NumWriteIO; //the number of pages written to the page file since the buffer pool has been initialized.

	int HitCount;   //when hit, increase by 1
	int HitTime; //hit time

	bool isDirty;  //is the page dirty?

} BM_PageHandle;


(4) Functions Introductions:
----------------------------------------------------------
*** create a buffer pool for an existing page file ***
1) RC initBufferPool(BM_BufferPool *const bm,const char *const pageFileName, const int numPages,ReplacementStrategy strategy, void *stratData);

initBufferPool creates a new buffer pool with numPages page frames using the page replacement strategy strategy.
The pool is used to cache pages from the page file with name pageFileName. 
Initially, all page frames should be empty. 
The page file should already exist, i.e., this method should not generate a new page file. 
StratData can be used to pass parameters for the page replacement strategy. 


2) RC shutdownBufferPool(BM_BufferPool *const bm);

shutdownBufferPool destroys a buffer pool. 
This method frees up all resources associated with buffer pool. 
It frees the memory allocated for page frames. 
If the buffer pool contains any dirty pages, then these pages was written back to disk before destroying the pool. 
If the pool that has pinned pages, this method would return error.

3) RC forceFlushPool(BM_BufferPool *const bm);

forceFlushPool causes all dirty pages (with fix count 0) from the buffer pool to be written to disk.

***pin pages, unpin pages, mark pages as dirty, and force a page back to disk ***
1) RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page);

markDirty marks a page as dirty by modify bool value associated with page.

2) RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page);

unpinPage unpins the page page. 
The pageNum field of page was used to figure out which page to unpin.
unpin simply decreases the fixcount of that page.

3) RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page);

forcePage writes the current content of the page back to the page file on disk.

4) RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum);

pinPage pins the page with page number pageNum. 
In this function, page selection, page replacement are implemented here.


*** make statistics about a buffer pool and its contents ***
1) PageNumber *getFrameContents (BM_BufferPool *const bm);

GetFrameContents returns an array of PageNumbers (of size numPages) where the ith element is the number of the page stored in the ith page frame. An empty page frame is represented using the constant NO_PAGE.

2) bool *getDirtyFlags (BM_BufferPool *const bm);

GetDirtyFlags function returns an array of bools (of size numPages) where the ith element is TRUE if the page stored in the ith page frame is dirty. Empty page frames are considered as clean.

3) int *getFixCounts (BM_BufferPool *const bm);

GetFixCounts function returns an array of ints (of size numPages) where the ith element is the fix count of the page stored in the ith page frame. Return 0 for empty page frames.

4) int getNumReadIO (BM_BufferPool *const bm);

GetNumReadIO returns the number of pages that have been read from disk since a buffer pool has been initialized. 

5) int getNumWriteIO (BM_BufferPool *const bm);

GetNumWriteIO returns the number of pages written to the page file since the buffer pool has been initialized.


----------------------------------------------------------

(5) Error types listed in dberror.h
----------------------------------
#define RC_BAD_PARAMETER 0xBAD

#define RC_OK 0

#define RC_FILE_NOT_FOUND 1
#define RC_FILE_HANDLE_NOT_INIT 2
#define RC_WRITE_FAILED 3
#define RC_READ_NON_EXISTING_PAGE 4
#define RC_FILE_ALREADY_EXISTS 5

#define RC_SM_FWRITE_ERROR 6
#define RC_SM_FREAD_ERROR 7
#define RC_SM_FOPEN_ERROR 8
#define RC_SM_FSEEK_ERROR 9
#define RC_SM_FCLOSE_ERROR 10

#define RC_FILE_SYS_ERROR 256 // OS System Call Error

#define RC_BM_NO_USABLE_PAGE 101
#define RC_BM_POOL_LOCKED 102
#define RC_BM_NON_EXISTING_PAGE 103

#define RC_RM_COMPARE_VALUE_OF_DIFFERENT_DATATYPE 200
#define RC_RM_EXPR_RESULT_IS_NOT_BOOLEAN 201
#define RC_RM_BOOLEAN_EXPR_ARG_IS_NOT_BOOLEAN 202
#define RC_RM_NO_MORE_TUPLES 203
#define RC_RM_NO_PRINT_FOR_DATATYPE 204
#define RC_RM_UNKOWN_DATATYPE 205

#define RC_IM_KEY_NOT_FOUND 300
#define RC_IM_KEY_ALREADY_EXISTS 301
#define RC_IM_N_TO_LAGE 302
#define RC_IM_NO_MORE_ENTRIES 303
--------------------------------------------------------------------------------------


(5) Optional Extensions
----------------------------------------------------------
Make the buffer pool functions thread safe. 
by using library pthreads, the operations involving writing are secured by mutex.

Additional page replacement strategies are implemented. 
See function FindPage_LFU and FindPage_Clock.

