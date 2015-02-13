#include "MetaServer.h"
#include <algorithm>

MetaServer::MetaServer() {
		// Initialize MetaServer receiver/sender objects
}

MetaServer::~MetaServer() {
		// destructor for MetaServer class object
}

void MetaServer::initiateMetaServer() {
		/*
			1. For each private object in MetaServer, open the connection to both Origin Server and FSS
			  - FSS will only send its file list when an update/creation of file(s) from the Client App is done

			2. Synchronize file list originally stored in FSS (should be served in a separate function?)

			origin_rcvr {
				recvfrom() {

					// syncFSS()
				}

			}

		*/
}

void MetaServer::endMetaServer() {
		// terminates the Meta Server's connection with others
}

	// receive request from FSS and save its information to Meta Server's database: file
	// TODO: do we want to get one file per connection or several files per connection?
void MetaServer::fileList_parser( string json ) {
		/*
			File list will be retrieved in json from HTTP
			Use this json information to extract (filename, filehash, CDNs)
			  and save it to some kind of file (database) on the Meta Server's machine
		   ...
		   ..
		   .
		*/
}

	/*
		The selection algorithm will be based on the file list input
		Using this file list received from the Origin Server, Meta Server will determine which CDN will serve each particular file
		When all the CDNs whose cache has the file are further away from FSS, our selection algorithm will choose the geographically closest CDN
	*/
	// return pair is <file_name, CDN address>
	// type: 0 = download, 1 = upload

vector< pair<string, string> > MetaServer::runSelectionAlgorithm( const vector< pair<string, string> >& fileListFromOrigin, int type ) {
		/*
			For each file in fileListFromOrigin
				1. Check if any CDN has that file and if its hash is the same, or not
					a. If so, check every CDN's distance from the Client and choose the closest CDN
					  - If all CDNs that have the file are more distant than FSS, get the closest CDN
					b. If none of CDNs have that file get the closest CDN
				2. Check the load of that CDN (tentative)
		*/
}

	// sorter for file list vector
void MetaServer::sortFileList( vector< pair<string, string> >& fileListFromOrigin ) {
		// sort the vector of <filename, filehash> in alphabetical order
		sort(fileListFromOrigin.begin(), fileListFromOrigin.end(), compare);
}

string MetaServer::getClosestCDN( const vector<string> cdn_list ) {
		// helper function for selectionAlgorithm that will determine which of the CDNs is the closest to the client
}

bool MetaServer::isCDN_closer( const string CDN_addr ) {
		// true = given CDN is closer to the client than the FSS
		// false = otherwise
}

bool MetaServer::CDN_load_OK( const string CDN_addr ) {
		// true = CDN is available to serve
		// false = too much load on that CDN; consider other
}

/*
// comparator function that sorts the file list in ascending alphabetical order
bool MetaServer::compare( pair<string, string>& a, pair<string, string>& b) {
		return a.first < b.first;
}
*/
