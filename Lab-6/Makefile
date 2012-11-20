all:
	$(MAKE) -C discoveryF4
	$(MAKE) -C hello-stm32
	$(MAKE) -C neocon

unit-test:
	$(CC) -o unit-test unit-test.c -lcunit

check: unit-test
	./unit-test

clean:
	$(MAKE) -C discoveryF4 clean
	$(MAKE) -C hello-stm32 clean
	$(MAKE) -C neocon clean
	rm -f unit-test
