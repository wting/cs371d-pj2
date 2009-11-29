#ifndef NETWORK_H
#define NETWORK_H

#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/lexical_cast.hpp>
#include "aux.h"
#include "logger.h"
#include "node.h"

using namespace std;
using aux::to_str;
using boost::asio::ip::tcp;

namespace network {
	enum { max_length = 1024 };

class client :
	//keep alive as long as something is pointing to this
	public boost::enable_shared_from_this<client> {
	boost::asio::io_service io;
	string host;
	string port;
	size_t buffer_length;

	logger* log;

public:
	client(string h, string p, logger* l) {
		log = l;
		log->write(2,"network::client()");
		host = h;
		port = p;
	}

	~client() {
		log->write(2,"network::~client()");
		log = 0; //do not delete log, should be handled by caller
	}

	string send(string msg) {
		try {
			log->write(4,to_str("network::client::send(): ") + msg);

			tcp::resolver resolver(io); //turns io into TCP endpoint
			tcp::resolver::query query(tcp::v4(), host, port); //constructing query
			tcp::resolver::iterator end_iterator = resolver.resolve(query); //turns query into list of endpoints
			tcp::resolver::iterator end;

			//creates socket and opens connection
			log->write(0,to_str("network::client::send() - creating socket to ") + host + to_str(":") + port);
			tcp::socket s(io);

			boost::system::error_code error = boost::asio::error::host_not_found;
			while (error && end_iterator != end) //iterate through all endpoints
			{
				s.close();
				s.connect(*end_iterator++, error);
			}
			if (error)
				throw boost::system::system_error(error);

			char ack[max_length];
			string reply;

			log->write(0,"network::client::send() - write buffer");
			boost::asio::write(s,boost::asio::buffer(msg,msg.size()));
			log->write(0,"network::client::send() - read  buffer");
			boost::asio::read(s,boost::asio::buffer(ack,msg.size()));
			reply = to_str(ack).substr(0,msg.size());
			log->write(3,to_str("network::client::send() - response: ") + to_str(reply));

			s.close();
			log->write(4,to_str("network::client::send() - send end"));
			return to_str(reply);
		} catch (exception& e) {
			cerr << "network::client::send() - exception:" << e.what() << endl;
		}

		return NULL;
	}
};

class session {
	tcp::socket socket_;
	char data_[max_length];

	logger* log;

public:
	session(boost::asio::io_service& io_service,logger* l) : socket_(io_service) {
		log = l;
		log->write(1,"network::session()");
	}

	tcp::socket& socket() { return socket_; }

	void start() {
		//read data from client
		log->write(1,"network::session::start()");
		socket_.async_read_some(boost::asio::buffer(data_, max_length),
				boost::bind(&session::handle_read, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));

	}

	/*char* timestamp(char* in) {
		string tmp = *in;
		tmp = "[timestamp]" + tmp;
		return tmp.c_str();
	}*/
	void timestamp() {
		strcat(data_,"_timestamp");
	}

	void handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
		log->write(1,"network::session::handle_read()");
		if (!error) {
			//write data to client
			timestamp();

			boost::asio::async_write(socket_,
					boost::asio::buffer(data_, bytes_transferred),
					boost::bind(&session::handle_write, this,
					boost::asio::placeholders::error));
		} else { delete this; }
	}

	void handle_write(const boost::system::error_code& error) {
		log->write(1,"network::session::handle_write()");
		if (!error) {
			socket_.async_read_some(boost::asio::buffer(data_, max_length),
					boost::bind(&session::handle_read, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		} else { delete this; }
	}
};

class server {
	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;

	short int port;
	dist::node* node;
	logger* log;

public:
	server(boost::asio::io_service& io_service, string p, logger* l) :
		//initializes io, acceptor to listen on port argument
		io_service_(io_service), acceptor_(io_service, tcp::endpoint(tcp::v4(), boost::lexical_cast<short int>(p))) {
			log = l;
			log->write(5,"network::server()");
			port = boost::lexical_cast<short int>(p);

			node = new dist::node(l);

			//creates new session for use with async
			session* new_session = new session(io_service_, log);

			//initiates async_accept operation to wait for new connections
			acceptor_.async_accept(new_session->socket(), boost::bind(&server::handle_accept, this, new_session, boost::asio::placeholders::error));
	}

	~server() {
		log->write(5,"network::~server()");
		delete node;
	}

	//event handler called by async_accept, services client request and then initiates next async_accept operation
	void handle_accept(session* new_session, const boost::system::error_code& error) {
		log->write(1,"network::server::handle_write()");
		if (!error) {
			new_session->start();
			new_session = new session(io_service_, log);
			acceptor_.async_accept(new_session->socket(), boost::bind(&server::handle_accept, this, new_session, boost::asio::placeholders::error));
		} else { delete new_session; }
	}
};

} // end namespace network

#endif // NETWORK_H
