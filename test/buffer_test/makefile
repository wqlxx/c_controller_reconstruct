test_cc_buffer : test_cc_buffer.o cc_buffer.o
	gcc -o test_cc_buffer -g test_cc_buffer.o cc_buffer.o

test_cc_buffer.o : test_cc_buffer.c cc_buffer.h
	gcc -g -c test_cc_buffer.c

cc_buffer.o : cc_buffer.h
	gcc -g -c cc_buffer.c

.PHONY : clean
clean :
	rm test_cc_buffer test_cc_buffer.o cc_buffer.o
