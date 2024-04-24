CXX = g++
CFLAGS = -Werror -Wall
DBGFLG = -g -fsanitize=address,undefined
CLIB = -lrt -lpthread

compile: init server client destroy

testing: bank.h server.cpp bank_d.o bank_cell_d.o config.h
	$(CXX) testing.cpp bank.o bank_cell.o $(CFLAGS) $(DBGFLG) -o testing $(CLIB)

init: bank.h init.cpp bank.o bank_cell.o config.h
	$(CXX) init.cpp bank.o bank_cell.o $(CFLAGS) -o init $(CLIB)

server: bank.h server.cpp bank.o bank_cell.o config.h
	$(CXX) server.cpp bank.o bank_cell.o $(CFLAGS) -o server $(CLIB)

client: client.cpp
	$(CXX) client.cpp $(CFLAGS) -o client $(CLIB)

destroy: bank.h destroy.cpp bank.o bank_cell.o config.h
	$(CXX) destroy.cpp bank.o bank_cell.o $(CFLAGS) -o destroy $(CLIB)

valgrind: init testing destroy
	./init
	valgrind ./testing
	./destroy


bank_cell.o: bank_cell.cpp bank.h
	$(CXX) bank_cell.cpp -c $(CFLAGS)

bank.o: bank_cell.h bank.h bank.cpp
	$(CXX) bank.cpp -c $(CFLAGS)

bank_cell_d.o: bank_cell.cpp bank.h
	$(CXX) bank_cell.cpp -c $(CFLAGS) $(DBGFLG) -o bank_cell_d.o $(CLIB)

bank_d.o: bank_cell.h bank.h bank.cpp
	$(CXX) bank.cpp -c $(CFLAGS) $(DBGFLG) -o bank_d.o $(CLIB)


.PHONY: clean
clean:
	rm -f init server destroy *.o testing client
