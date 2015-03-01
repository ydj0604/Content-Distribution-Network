#define CDN_DIR "./cdn/cache"
#define CDN_ADDR "http://localhost:5000"

struct Address {
	pair<double, double> latLng; //always keep 2 decimal digits for latitude and longitude
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

typedef struct Address Address;
