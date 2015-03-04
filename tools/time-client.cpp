#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sysexits.h>

#include <iostream>
#include <chrono>

#define MAXBUF 65536

using hi_res_clock = std::chrono::high_resolution_clock;

int main(int argc, char* argv[])
{
	// check for port argument
	if(argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
		exit(EX_USAGE);
	}

	// print out clock accuracy
	std::cout << (double) hi_res_clock::period::num / hi_res_clock::period::den
		<< " seconds per clock tick" << std::endl;

	// set up socket
	struct sockaddr_in sock_in = {0};
	unsigned int sinlen = sizeof(struct sockaddr_in);

	int sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock < 0) {
		perror("Couldn't create socket");
		exit(1);
	}

	int yes = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	sock_in.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	sock_in.sin_port = htons(atoi(argv[1]));
	sock_in.sin_family = AF_INET;

	if(bind(sock, (struct sockaddr *)&sock_in, sinlen) != 0) {
		perror("Couldn't bind to socket");
		exit(1);
	}

	// ready to receive
	while(true) {

		// recieve into array
		uint32_t index;
		uint64_t time;

		uint8_t data[sizeof(index) + sizeof(time)];
	
		size_t bytes_recv = recvfrom(sock, data, sizeof(data), 0,
			(struct sockaddr *)&sock_in, &sinlen);

		auto now = hi_res_clock::now().time_since_epoch().count();

		if(bytes_recv == 0) {
			break;
		} else if(bytes_recv < 0) {
			perror("Couldn't receive bytes");
			exit(2);
		} else if (bytes_recv < sizeof(data)) {
			std::cerr << "Not all bytes were received..." << std::endl;
		}
		
		// print out values
		memcpy(&index, &data[0], sizeof(index));
		memcpy(&time, &data[sizeof(index)], sizeof(time));

		auto dif = now - time;
		std::cout << index << ": " << time << ", " << now << ", " << dif << std::endl;

	}

	shutdown(sock, SHUT_RDWR);
	close(sock);
}
