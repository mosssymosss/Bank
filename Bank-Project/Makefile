CXX = g++
CFLAGS = -Werror -Wall
DBGFLG = -g -fsanitize=address,undefined
CLIB = -lrt -lpthread


.PHONY: run
run: init client destroy
	./init
	./client
	./destroy

.PHONY: run_client
run_client: client
	./client

testing: bank.h client.cpp bank_d.o bank_cell_d.o
	$(CXX) testing.cpp bank.o bank_cell.o $(DBGFLG) -o testing $(CLIB)

init: bank.h init.cpp bank.o bank_cell.o
	$(CXX) init.cpp bank.o bank_cell.o $(CFLAGS) -o init $(CLIB)

client: bank.h client.cpp bank.o bank_cell.o
	$(CXX) client.cpp bank.o bank_cell.o $(CFLAGS) -o client $(CLIB)

destroy: bank.h destroy.cpp bank.o bank_cell.o
	$(CXX) destroy.cpp bank.o bank_cell.o $(CFLAGS) -o destroy $(CLIB)

valgrind: init testing destroy
	./init
	valgrind ./testing
	./destroy


bank_cell.o: bank_cell.cpp bank.h
	$(CXX) bank_cell.cpp -c

bank.o: bank_cell.h bank.h bank.cpp
	$(CXX) bank.cpp -c

bank_cell_d.o: bank_cell.cpp bank.h
	$(CXX) bank_cell.cpp -c $(DBGFLG) -o bank_cell_d.o $(CLIB)

bank_d.o: bank_cell.h bank.h bank.cpp
	$(CXX) bank.cpp -c $(DBGFLG) -o bank_d.o $(CLIB)


.PHONY: clean
clean:
	rm -f init client destroy *.o init_d client_d destroy_d testing
