#ifndef CDN_NODE_H
#define CDN_NODE_H

#include <vector>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <utility>
#include "CDN_Cache.h"
#include <string>
#include <sys/stat.h> //for creating directory
#include <dirent.h> //for reading directory

class CDNSender;

using namespace std;

class CDN_Node {
public:
    
	CDN_Node(string metaIpAddr, string fssIpAddr);
	~CDN_Node();
    
	//utility functions
	bool make_storage();
	bool look_up_and_remove_storage(string filename, int signal);
    void managing_files(long long file_size);
    long long get_size_of_storage();
    char* path_maker(const char* name);

    //added
    void startListening();
    void endListening();
    CDNSender* getSender() { return m_sender; }
    
    //gps functions
    void get_and_set_CDN_addr();
    pair <double, double> get_gps_info ();

private:

    DIR *dir;
    struct dirent *ent;
    
    long long storage_capacity = 10000000; //the capacity is 10MB
	long CDN_addr;	// CDN's IP address
    char wd[256];  // storage directory path

	long long size_of_storage = 0; //Count the number of files in CDN Nodes
	//bool is_locked;
    pair <double, double> cdn_gps; //Location info for CDN
    LRUCache file_tracker; //Storing the information of files in the order

    //added
    int m_cdnId;
    Address m_address;
    string m_metaIpAddr;
    string m_fssIpAddr;
    CDNSender* m_sender;
	
};

#endif



