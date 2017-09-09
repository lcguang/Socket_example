#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

int main() {
    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "error opening stream socket" << std::endl;
        exit(1);
    }
    
    // Bind socket using wildcards
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = 0;
    if (bind(sockfd, (sockaddr *) &server, sizeof(server)) == -1) {
        std::cerr << "error binding stream socket" << std::endl;
        exit(1);
    }
    
    // Find out assigned port number and print it out
    socklen_t length = sizeof(server);
    if (getsockname(sockfd, (sockaddr *) &server, &length) == -1) {
        std::cerr << "error getting socket name" << std::endl;
        exit(1);
    }
    
    std::cout << "Socket port " << ntohs(server.sin_port) << std::endl;
    
    // Start accepting connections
    listen(sockfd, 10);
    int msg_fd = accept(sockfd, nullptr, nullptr);
    if (msg_fd == -1) {
        std::cerr << "error accepting connection" << std::endl;
        exit(1);
    }

    // Create buffer and receive message
    std::string answer = "1234";
    char recv_data[1024];
    while (true) {
        memset(recv_data, 0, sizeof(recv_data));
        int recv_len = 0;
        while (recv_len == 0) {
            recv_len = recv(sockfd, recv_data, sizeof(recv_data), 0);
        }
        if (recv_len == -1) {
            std::cerr << "error receiving message" << std::endl;
        }

        int count = 0;
        for (int i = 0; i < 4; i++) {
            if (recv_data[i] == answer[i]) count++;
        }
        if (count < 4) {
            std::cout << "Correct number of digits: " << count << std::endl;
        } else {
            std::cout << "Success!" << std::endl;
            auto send_data = std::to_string(count).c_str();
            send(msg_fd, send_data, strlen(send_data), 0);
            break;
        }

        // Send message
        auto send_data = std::to_string(count).c_str();
        send(msg_fd, send_data, strlen(send_data), 0);
    }

    close(msg_fd);
    return 0;
}
