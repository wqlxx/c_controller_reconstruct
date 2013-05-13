#VPATH = ../../include:../../lib
vpath %.h ../../include
vpath %.c ../../lib

test_cc_thread_pool : test_cc_thread_pool.o cc_thread_pool.o cc_queue.o cc_buffer.o cc_log.o
	gcc -o $@ -g test_cc_thread_pool.o cc_thread_pool.o cc_queue.o cc_buffer.o cc_log.o -lpthread

test_cc_thread_pool.o : test_cc_thread_pool.c cc_thread_pool.h cc_queue.h cc_buffer.h
	gcc -g -c test_cc_thread_pool.c -I ../../include

cc_thread_pool.o : cc_thread_pool.c cc_thread_pool.h cc_error.h
	gcc -g -c cc_thread_pool.c -I ../../include

cc_queue.o : cc_queue.c cc_queue.h cc_buffer.h cc_error.h cc_log.h
	gcc -g -c cc_queue.c -I ../../include

cc_buffer.o : cc_buffer.c cc_buffer.h 
	gcc -g -c cc_buffer.c -I ../../include

cc_log.o : cc_log.c cc_log.h
	gcc -g -c cc_log.c -I ../../include

.PHONY : clean
clean :
	-rm test_cc_thread_pool *.o
