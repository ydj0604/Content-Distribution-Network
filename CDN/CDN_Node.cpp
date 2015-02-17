#include "CDN_Node.h"
#include <string>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <fstream>
#include <iostream>


using namespace std;


CDN_Node::CDN_Node() {
    //initialize all variables and objects
		//is_locked = false;

		//creating file directory
        make_storage();
    cout << "test" << endl;

}

CDN_Node::~CDN_Node() {
		//clear out all components of CDN Node
}

bool CDN_Node::make_storage() {
		/* creating the file directory*/
        //getcwd( wd, 256 );  // give that buffer to getcwd and tell it how big the buffer is.

        
        /*
		// if you want to put that data in a string...
		string cwd = wd;  // like "long double main" suggested

		//assign the path for creating new directory
		file_path = cwd;
		file_path.append("/cdn_node");
        */
        
        strcpy(wd, "/Users/wonjaelee/Desktop");
        strcat(wd, "/cdn_node");
     
		//making new directory
		mkdir(wd,7777);
    
        cout << wd << endl;

		//if there is no error, return true
		return true;

}


bool CDN_Node::look_up_and_update_storage(string filename, int signal) {
		/*
			Given filename, and signal is 0, look up the storage of Node and return true if there is match.
		
            If signal is 1, look up the file and remove it from the directory.
         */
        
            //convert string to char
            const char* cstr = filename.c_str();
       
            //make the name for removing
            char remove_file[256];
            strcpy(remove_file, wd);
            strcat(remove_file,"/");
            strcat(remove_file, cstr);
        
			if((dir = opendir(wd)) != NULL) {
                cout << " serious" << endl;
				while ((ent = readdir(dir)) != NULL) {
					if (strcmp(ent->d_name,cstr)==0 && signal == 0) {
						return true;
                    } else if (strcmp(ent->d_name,cstr)==0 && signal == 1) {
                        //remove the file
                        if(remove(remove_file) == 0) {
                            cout << "deletion successful" << endl;
                        }
                    }
				}
				closedir (dir);
			} else {
				/* can not open directory */
				perror("Can't open the directory");
				return EXIT_FAILURE;
			}

			return false;
}

/*
	bool CDN_Node::update_storage(string filename){
        
        if(look_up_storage(filename) == true){
            
            //convert string to char
            const char* cstr = filename.c_str();
            
            if((dir = opendir(wd)) != NULL) {
                remove(cstr);
            }
                closedir (dir);
            
        }
 
			When new information about the file in the cache is received, replace that file to new one or just clear it from the cache.
 
        return true;
	}
*/

bool CDN_Node::trasfer_file_to_clients(string client_address, string filename, int filehash) {
		/*
			When Origin Server request the transfer the certain file to Clients, sending it to client_address directly.
		*/
        return true;
}

bool CDN_Node::get_file_from_storage (string filename, int filehash){

		/*
			If the file can be get from the cache, dispatch it and return the file.
		*/
        return true;
}

bool CDN_Node::get_file_from_FSS (string filename, int filehash){
		
		/*
			Get the file from FSS and send to client. Then, save it to the Cache.
		*/
        return true;
}

bool CDN_Node::save_file_to_FSS (string filename, int filehash){
		
			/* 
				Get the file from storage and send to FSS.
			*/
        return true;
}
	
void CDN_Node::managing_files() {
		/*
			Manage the file storage to maintain its free-capacity.
		*/
}

long long CDN_Node::get_size_of_storage() {
    struct stat sb;
    if((dir = opendir(wd)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            cout << ent->d_name << endl;
            
            
            //make the name for calculating the size
            char size_file[256];
            strcpy(size_file, wd);
            strcat(size_file,"/");
            strcat(size_file, ent->d_name);
            
            stat(size_file, &sb);
            cout << "size of file: " << sb.st_size << endl;
            size_of_storage += (long long)sb.st_size;
        }
        closedir (dir);
    } else {
        /* can not open directory */
        perror("Can't open the directory");
        return EXIT_FAILURE;
    }
        return size_of_storage;
}


/*
	void CDN_Node::lock_the_storage(){
		is_locked = true;
	}

	void CDN_Node::unlock_the_storage(){
		is_locked = false;
	}
*/



