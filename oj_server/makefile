.PHONY:oj_server
oj_server:oj_server.cpp
	g++ $^ -o $@ -std=c++11 -ljsoncpp -pthread  -lctemplate -lmysqlclient
# -I./include -L./lib
.PHONY:clean
clean:
	rm -f oj_server

