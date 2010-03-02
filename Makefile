test: test.c
	gcc -Wall test.c -o test -lpthread	

.PHONY:clean

clean:
	rm -fr *.txt test
