#include <cstdlib>
#include <iostream>

#include "network.h"

using namespace std;

int main(int argc char* argv[]) {
	try
	{
		if (argc != 2) {
			std::cerr << "Usage: async_tcp_echo_server <port>\n";
			return 1;
		}

		boost::asio::io_service io;
		network::server s(io, atoi(argv[1]));
		io_service.run();
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
