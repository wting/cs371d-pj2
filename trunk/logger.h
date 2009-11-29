#ifndef LOGGER_H
#define LOGGER_H

#include <iomanip>		//setw
#include <iostream>		//cout, endl
#include <fstream>		//istream, ostream
#include <stdexcept>
#include <string>
#include <unistd.h>		//sleep

#include "aux.h"

using namespace std;
using aux::to_str;

class logger {
	bool display;
	int threshold;
	int num_lines;
	//ostringstream stream;
	string file;
	ofstream file_w;
	ifstream file_r;

	int process_id;

	template <typename T>
	void print_error(const T &input) const {
		cerr << process_id << "[xxxx][xxx]: " << input << endl;
	}

public:
	///\TODO: add custom stream to write to
	//logger(bool disp, int thresh, ostringstream &out, string file = "") { }
	logger() {
		display = true;
		threshold = 0;
		num_lines = 0;
		file = "";
		process_id = 0;
	}

	logger(bool disp, int pid = 0, int thresh = 0) {
		display = disp;
		process_id = pid;
		threshold = thresh;
		num_lines = 0;

		file = to_str("pid_") + to_str(pid) + to_str(".log");
		file_w.open(file.c_str());
		if (file_w.fail()) {
			file_w.close();
			//throw runtime_error(to_str("Cannot create write log file") + file);
			print_error(to_str("Cannot create write log file") + file);
		}
	}

	~logger() {
		//cout << "\n~logger()" << endl;
		//this->read();
		file_w.close();
		file_r.close();
		if (remove(file.c_str()))
			//cerr << "Error deleting file: " << file << endl;
			print_error(to_str("Error deleting file: ") + to_str(file));
	}

	///display entire stable storage
	void read() {
		file_r.open(file.c_str());
		if (file_r.fail()) {
			file_r.close();
			//throw runtime_error("Cannot open write log file.");
			print_error(to_str("Cannot open write log file") + file);
		}

		int n=0;
		string line;
		while (!file_r.eof()) {
			getline(file_r,line);
			if (file_r.good())
				print(++n, line);
		}

		file_r.close();
	}

	///display first n lines of stable storage
	void read_head(const int &num) {
		file_r.open(file.c_str());
		if (file_r.fail()) {
			file_r.close();
			//throw runtime_error("Cannot open write log file.");
			print_error(to_str("Cannot open write log file") + file);
		}

		int n=1;
		string line;
		while (n <= num && !file_r.eof()) {
			getline(file_r,line);
			print(n++,line);
		}

		file_r.close();
	}

	///display last n lines of stable storage
	void read_tail(const int &num) {
		file_r.open(file.c_str());
		if (file_r.fail()) {
			file_r.close();
			//throw runtime_error("Cannot open write log file.");
			print_error(to_str("Cannot open write log file") + file);
		}

		//count number of lines
		int size = -1;
		string line;
		while (!file_r.eof()) {
			getline(file_r,line);
			++size;
		}

		//display last num lines
		file_r.close();
		file_r.open(file.c_str());
		int n = 1;
		//file_r.seekg(0);
		//file_r.clear();
		while (!file_r.eof()) {
			getline(file_r,line);
			if (n > (size - num) && file_r.good())
				print(n,line);
			++n;
		}

		file_r.close();
	}

	///display past n lines of stable storage above threshold
	void read(int thres, int n) {

	}

	template <typename T>
	void set_file(const T &input) {
		file_w.close();
		file_r.close();
		file = to_str(input);
		file_w.open(file.c_str());
		if (file_w.fail()) {
			file_w.close();
			//throw runtime_error("Cannot create write log file.");
			print_error(to_str("Cannot create write log file") + file);
		}
	}

	template <typename T>
	void write(int thresh, const T &input) {
		if (thresh >= threshold)
			print(num_lines+1, thresh, input);
		write(input);
	}

	template <typename T>
	void write(const T &input) {
		///\FIXME: incorrect file causes seg fault
		file_w.clear();
		file_w << input << endl;
		++num_lines;
		if (file_w.fail()) {
			file_w.close();
			print_error(to_str("Cannot write to log file") + file);
		}
	}

private:
	template <typename T>
	void print(const int &n, const T &input) const {
		print(n,-1,input);
	}

	template <typename T>
	void print(const int &n, const int &t, const T &input) const {
		if (t == 100)
			cout << "ERROR: ";
		/*if (t >= 0) {  //adds tabs for different priority levels
			int tab = t;
			if (tab > 5)
				tab = 5;

			while (tab < 5) {
				cout << "    ";
				++tab;
			}

			cout << process_id << "[" << setw(4) << n << "][" << t << "]: " << input << endl;
		} else*/
			cout << process_id << "[" << setw(4) << n << "]: " << input << endl;
	}
};

#endif // LOGGER_H
