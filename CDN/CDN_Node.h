#ifndef CDN_NODE_H
#define CDN_NODE_H

#include <vector>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <utility>
#include "CDN_Cache.h"
#include "Shared.h"
#include <string>
#include <sys/stat.h> //for creating directory
#include <dirent.h> //for reading directory

class CDNSender;

using namespace std;

class CDN_Node {
public:
    
	CDN_Node(string cdnIpAddr, string metaIpAddr, string fssIpAddr, string city="la");
	~CDN_Node();
    
	//utility functions
	bool make_storage();
	bool look_up_and_remove_storage(string filename, int signal);
    bool managing_files(string filename, long long file_size, vector<string>& deletedfiles);
    long long get_size_of_storage();
    char* path_maker(const char* name);

    //added
    void startListening();
    void endListening();
    CDNSender* getSender() { return m_sender; }
    
    //added after
    void get_address();
    string get_fss_ip() { return m_fssIpAddr; }
    string get_meta_ip() { return m_metaIpAddr;}
    int get_cdn_id() {return m_cdnId;}
    bool write_file (const string& contents, string filename,vector<string>& deletedfiles);
    string load_file (string filename);
    bool delete_file (string filename);
    
    //gps functions
    void get_and_set_CDN_addr();
    pair <double, double> get_gps_info ();
private:

    DIR *dir;
    DIR* dir_2;
    struct dirent *ent;
    struct dirent * ent_2;
    
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
    string m_metaIpAddr; //including port
    string m_fssIpAddr; //including port
    CDNSender* m_sender;
	
};

#endif



