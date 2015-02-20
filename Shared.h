#ifndef SHARED_H
#define SHARED_H

#include <string>
using namespace std;

struct Address {
	pair<double, double> latLng;
	string ipAddr;
	Address() {
		latLng = make_pair(0.0, 0.0);
		ipAddr = "";
	}
	Address(pair<double, double> p, string s) {
		latLng = p;
		ipAddr = s;
	}
};

#endif
