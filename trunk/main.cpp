/**
	\file main.cpp
	\brief CS 371D Project - Playlist Editing

	\author HanPing Ting
	\author Will Warner
*/

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <boost/asio.hpp>

#include "aux.h"
#include "logger.h"
#include "node.h"
#include "network.h"

using namespace std;
using namespace aux;

volatile bool child_exit = false;

void sig_child(int sig) {
	cout << "main()::caught signal " << sig << endl;
	cout << "\tpid = " << getpid() << endl;
	child_exit = true;
}

///\TODO: log cleanup
int main(int argc, char* argv[]) {
	//(void) signal(SIGINT,sig_child); //finish signal catching
	///\FIXME: possible memory leaks whene exiting with ctrl+d

	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " <port>" << endl;
		return 1;
	}

	dist::node n();
	pid_t parent_ID = getpid();
	pid_t c_id = fork();
	if (c_id > 0) { //parent process
		logger log(0,parent_ID);
		log.write(5,"main()::parent process begin");
		log.write(0,to_str("main()::forked child pid: ") + to_str(c_id));

		{
			log.write(0,to_str("main()::client creation to port ") + to_str(argv[1]));
			network::client cl("localhost",argv[1],&log);
			string line;
			//log.write(2,"beginning input block");
			while (true) {
				int c_status = 0;
				waitpid(-1,&c_status,WNOHANG);
				if (c_status != 0) {
					log.write(100,"main()::problem with child process detected");
					break;
				}

				cout << endl;
				cout << "input: " << endl;
				getline(cin,line);

				if (cin.eof() || line.compare("exit") == 0)
					break;
				log.write(4,to_str("main()::sending input to localhost:" + to_str(argv[1]) + ": ") + line);
				cl.send(line);
			}
		}

		log.write(4,"main()::request child process to terminate");
		//send command to network socket
		kill(c_id,SIGINT);
		//wait 3 sec, check for existence of child?  send kill command
		waitpid(c_id,0,0);

		log.write(5,"main()::parent process end");
	} else if (c_id == 0) { //child process
		logger log(0,getpid());
		log.write(5,"main()::child process begin");
		boost::asio::io_service io;
		//dist::node n();

		try {
			log.write(3,to_str("main()::server creation on port ") + to_str(argv[1]));
			network::server s(io,argv[1],&log);

			log.write(3,"main()::server running");
			io.run();

			log.write(3,"main()::server exiting");
		} catch (...) {
			log.write(100,"main()::server creation failed");
			log.write(5,"main()::child process end");
			exit(1);
			throw;
		}

		log.write(5,"main()::child process end");
	} else {
		cerr << "main()::failed to fork" << endl;
		exit(1);
	}

    return 0;
}
