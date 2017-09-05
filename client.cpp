#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

int main(int argc, char *argv[]) {
    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "error opening stream socket" << std::endl;
        exit(1);
    }
    
    // Connect socket using name specified by command line
    sockaddr_in server;
    server.sin_family = AF_INET;
    
    // gethostbyname() returns a struct including the network address of the specified host
    hostent * host = gethostbyname(argv[1]);
    if (host == nullptr) {
        std::cerr << argv[1] << ": unknown host" << std::endl;
        exit(1);
    }
    memcpy(&server.sin_addr, host->h_addr, host->h_length);

    server.sin_port = htons(atoi(argv[2]));

    // Connect to server
    if (connect(sockfd, (struct sockaddr *) &server, sizeof(server)) == -1) {
        std::cerr << "error connecting stream socket" << std::endl;
        exit(1);
    }

    // Send message
    auto send_data = "Hello, server!";
    std::cout << "Outgoing message: " << send_data << std::endl;
    if (send(sockfd, send_data, strlen(send_data) + 1, 0) == -1) {
        std::cerr << "error sending on stream socket" << std::endl;
    }

    // Create buffer and receive message
    char recv_data[1024];
    memset(recv_data, 0, sizeof(recv_data));
    int recv_len = recv(sockfd, recv_data, sizeof(recv_data), 0);
    if (recv_len == -1) {
        std::cerr << "error receiving message" << std::endl;
    }
    std::cout << "Incoming message: " << recv_data << std::endl;

    close(sockfd);
    return 0;
}