all:
	gcc -Wall --coverage -g hello.c -o hello

run:
	killall -9 hello 2>/dev/null || @echo
	./hello &
	sleep 3
	killall -USR1 hello

clean:
	rm -f hello \
		*.o \
		*.gcov  *.gcda  *.gcno 
