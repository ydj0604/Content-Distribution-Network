#ifndef CDN_NODE_H
#define CDN_NODE_H

#include <vector>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <utility>
#include "CDN_Cache.h"

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

	bool look_up_and_remove_storage(string filename, int signal);

	bool transfer_file_to_clients(string client_address, string filename);
    bool transfer_file_from_clients(string client_address, string filename, int filehash, long long filesize);
    
    bool get_file_from_FSS (string filename, long long filesize);
    bool save_file_to_FSS (string filename, int filehash);
    
    
	bool get_file_from_storage (string filename, int filehash);
    void save_file_to_storage (string filename, long long filesize);

    
    
    void managing_files(long long file_size);
    long long get_size_of_storage();
    char* path_maker(const char* name);
    
    /* 
    should implement meta relating functions
    */
    
	
    
    //about gps information
    void get_and_set_CDN_addr();
    pair <double, double> get_gps_info ();
    
    
    //not yet planned to implement
    void lock_the_storage();
    void unlock_the_storage();


/*
To do
1.let metaserver knows whether the file is update/upload/download
*/

private:

    DIR *dir;
    struct dirent *ent;
    
    long long storage_capacity = 10000000; //the capacity is 10MB
	long CDN_addr;	// CDN's IP address
    char wd[256];  // use an actual buffer, not a pointer
	//string file_path; //storage path name

	long long size_of_storage = 0; //Count the number of files in CDN Nodes
	//bool is_locked;
    pair <double, double> cdn_gps; //Location info for CDN
    LRUCache file_tracker; //Storing the information of files in the order
	
};

#endif


