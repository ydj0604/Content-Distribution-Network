#ifndef CDN_NODE_H
#define CDN_NODE_H

#include <vector>
#include <fstream>
#include <iostream>
#include <stdio.h>
/*
#include "OriginServerReceiver.h"
#include "OriginServerSender.h"
#include "FSSReceiver.h"
#include "FSSSender.h"
#include "MetaServer.h"
*/

//for creating directory
#include <sys/stat.h>

//for reading directory
#include <dirent.h>

using namespace std;

class CDN_Node {
public:
	CDN_Node();
	~CDN_Node();
	bool make_storage();

	bool look_up_and_update_storage(string filename, int signal);

	bool update_storage(string filename);
	bool trasfer_file_to_clients(string client_address, string filename, int filehash);

	bool get_file_from_storage (string filename, int filehash);
	bool get_file_from_FSS (string filename, int filehash);
	bool save_file_to_FSS (string filename, int filehash);

    long long get_size_of_storage();
    
	void managing_files();
	void lock_the_storage();
	void unlock_the_storage();

/*
To do
1.let metaserver knows whether the file is update/upload/download
*/

private:

    DIR *dir;
    struct dirent *ent;
    
	int CDN_addr;	// CDN's IP address
    char wd[256];  // use an actual buffer, not a pointer
	//string file_path; //storage path name

	long long size_of_storage = 0; //Count the number of files in CDN Nodes
	//bool is_locked;

	
};

#endif



