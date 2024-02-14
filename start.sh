# g++ -c Server.cpp -o Server.o
# g++ -c http_tcpServer.cpp -o http_tcpServer.o
# g++  Server.o http_tcpServer.o -o myServer.out

g++ Server.cpp http_tcpServer.cpp -o myServer.out
./myServer.out