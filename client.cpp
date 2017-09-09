#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_set>

int getResponse(int* sockfd, const char* send_data) {
    std::cout << "My guess: " << send_data << std::endl;
    if (send(*sockfd, send_data, strlen(send_data) + 1, 0) == -1) {
        std::cerr << "error sending on stream socket" << std::endl;
    }

    // Create buffer and receive message
    char recv_data[10];
    memset(recv_data, 0, sizeof(recv_data));
    int recv_len = 0;
    while (recv_len == 0) {
        recv_len = recv(*sockfd, recv_data, sizeof(recv_data), 0);
    }
    if (recv_len == -1) {
        std::cerr << "error receiving message" << std::endl;
    }

    std::cout << "Correct number of digits: " << recv_data << std::endl;
    
    return recv_data[0] - '0';
}

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

    // Main algorithm
    std::unordered_set<int> nums;
    for (int i = 1; i < 7; i++) {
        std::string send_data = std::to_string(i * 1111);
        int count = getResponse(&sockfd, send_data.c_str());
        std::cout << count << std::endl;
        if (count == 4) {
            close(sockfd);
            return 0;
        } else if (count != 0) {
            nums.insert(i);
        }
    }

    std::vector<int> pos(4, 0);
    for (auto i : nums) {
        for (int j = 0; j < 4; j++) {
            if (pos[j] != 0) continue;
            std::string send_data = "777";
            send_data.insert(j, 1, '0' + i);
            int count = getResponse(&sockfd, send_data.c_str());
            if (count == 1) pos[j] = i;
        }
    }

    std::string send_data = "";
    for (int i = 0; i < 4; i++) send_data.push_back('0' + pos[i]);
    if (getResponse(&sockfd, send_data.c_str()) == 4) {
        std::cout << "Success!" << std::endl;
    }

    close(sockfd);
    return 0;
}