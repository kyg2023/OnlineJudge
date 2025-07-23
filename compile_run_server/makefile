.PHONY:compile_run_server
compile_run_server:compile_run_server.cpp
	g++ $^ -o $@ -std=c++11 -ljsoncpp -pthread

.PHONY:clean
clean:
	rm -f compile_run_server