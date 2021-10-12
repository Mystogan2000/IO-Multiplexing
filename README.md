# IO-Multiplexing For Linux
1. Select 2. Poll 3. Epoll

Server:
gcc -o s select.c
./s port_no

Client:
Telnet localhost port_no
