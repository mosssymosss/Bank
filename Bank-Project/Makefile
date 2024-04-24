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


init: bank.h init.cpp bank.o bank_cell.o config.h
	$(CXX) init.cpp bank.o bank_cell.o $(CFLAGS) -o init $(CLIB)

client: bank.h client.cpp bank.o bank_cell.o config.h
	$(CXX) client.cpp bank.o bank_cell.o $(CFLAGS) -o client $(CLIB)

destroy: bank.h destroy.cpp bank.o bank_cell.o config.h
	$(CXX) destroy.cpp bank.o bank_cell.o $(CFLAGS) -o destroy $(CLIB)

testing: bank.h client.cpp bank_d.o bank_cell_d.o config.h
	$(CXX) testing.cpp bank_d.o bank_cell_d.o $(CFLAGS) $(DBGFLG) -o testing $(CLIB)


valgrind: init testing destroy
	./init
	valgrind ./testing
	./destroy


bank_cell.o: bank_cell.cpp bank.h
	$(CXX) bank_cell.cpp -c $(CFLAGS)

bank.o: bank_cell.h bank.h bank.cpp
	$(CXX) bank.cpp -c $(CFLAGS)

bank_cell_d.o: bank_cell.cpp bank.h
	$(CXX) bank_cell.cpp -c $(CFLAGS) $(DBGFLG) -o bank_cell_d.o

bank_d.o: bank_cell.h bank.h bank.cpp
	$(CXX) bank.cpp -c $(CFLAGS) $(DBGFLG) -o bank_d.o


.PHONY: clean
clean:
	rm -f init client destroy *.o init_d client_d destroy_d testing
