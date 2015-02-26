#include "CDN_Node.h"
#include "csv.h" //use CSV parser
#include "CDN_Cache.h"
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
    
    //set the external IpAddress and convert into number for location
    //get_and_set_CDN_addr();
    
    file_tracker.set("IMG_0428.JPG", 1500000);
    file_tracker.set("IMG_0429.JPG", 1400000);
    file_tracker.set("IMG_0433.JPG", 1800000);
    file_tracker.set("IMG_0436.JPG", 1600000);
    file_tracker.set("IMG_0496.JPG", 1500000);
    file_tracker.set("IMG_0620.JPG", 1600000);
    file_tracker.set("IMG_0629.JPG", 1800000);

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
        //strcpy(wd, "./cdn_node");
    
    
		//making new directory
		mkdir(wd,7777);
    
        //cout << wd << endl;

		//if there is no error, return true
		return true;

}


bool CDN_Node::look_up_and_remove_storage(string filename, int signal) {
		/*
			Given filename, and signal is 0, look up the storage of Node and return true if there is match.
		
            If signal is 1, look up the file and remove it from the directory.
         */
        
            //convert string to char
            const char* cstr = filename.c_str();
        
			if((dir = opendir(wd)) != NULL) {
				while ((ent = readdir(dir)) != NULL) {
					if (strcmp(ent->d_name,cstr)==0 && signal == 0) {
                        cout << cstr << " is located in this directory" << endl;
						return true;
                    } else if (strcmp(ent->d_name,cstr)==0 && signal == 1) {
                        //remove the file
                        char temp[256];
                        strcpy(temp,path_maker(cstr));
                        if(remove(temp) == 0) {
                            cout << "deletion successful" << endl;
                            return true;
                        } else {
                            cout << "Can't remove " << cstr << ": " << strerror(errno) << endl;
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



bool CDN_Node::transfer_file_to_clients(string client_address, string filename) {
		/*
			When Origin Server request the transfer the certain file to Clients, sending it to client_address directly.
		*/
        file_tracker.get(filename);
        return true;
}

bool CDN_Node::transfer_file_from_clients(string client_address, string filename, int filehash, long long filesize){
    
    file_tracker.set(filename, filesize);
    return true;
}

bool CDN_Node::get_file_from_FSS (string filename, long long filesize){
		
		/*
			Get the file from FSS and send to client. Then, save it to the Cache if possible.
		*/
        file_tracker.set(filename, filesize);
        return true;
}

bool CDN_Node::save_file_to_FSS (string filename, int filehash){
		
			/* 
				Get the file from storage and send to FSS.
			*/
        return true;
}


bool CDN_Node::get_file_from_storage (string filename, int filehash){
    
    /*
     If the file can be get from the storage, dispatch it and return the file.
     */
    return true;
}

void CDN_Node::save_file_to_storage (string filename, long long filesize){
    
    //To Do
    //get the file and save it into the directory
    
    
    
    //store the file information to filetracker
    file_tracker.set(filename, filesize);
    return;
}


/*
 Whenever we try to save the file into Directory, CDN takes the new file size and compare it whether
 the file size fits to the capacity or not. If it exceeds the limit, this function remove least used
 files until it satisfy the condition.
 */
void CDN_Node::managing_files(long long file_size) {
		/*
			Manage the file storage to maintain its free-capacity.
		*/
    while((this->get_size_of_storage() + file_size) > storage_capacity) {
        cout << "current capacity: " << this->get_size_of_storage() << endl;
        string file_name = file_tracker.remove();
        this->look_up_and_remove_storage(file_name, 1);
    }
    
    cout << "Current storage capacity is: " << this->get_size_of_storage() << endl;
    
}

/*
 Get the size of the directory
*/
long long CDN_Node::get_size_of_storage() {
    size_of_storage = 0;
    struct stat sb;
    if((dir = opendir(wd)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            stat(path_maker(ent->d_name), &sb);
            //cout << "size of file: " << sb.st_size << endl;
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
 Build the path c_string
 */
char* CDN_Node::path_maker(const char* name){
    char path_file[256];
    strcpy(path_file, wd);
    strcat(path_file, "/");
    
    return strcat(path_file, name);

}


/*
 Before get the gps information, we have to convert IP address into some certain numbers that fit into
 GPS lookup table. So we have to get external IP address first. Then split and convert it into integers.
 */

void CDN_Node::get_and_set_CDN_addr() {
    string line;
    ifstream IPFile;
    int n = 0;
    int i = 0;
    int m = 0;
    string test;
    string s;
    vector <int> store(4);
    
    system("curl ipecho.net/plain > ip.txt");
    
    IPFile.open ("ip.txt");
    
    if(IPFile.is_open())
    {
        getline(IPFile,line);
        string test = line;
        
        //get the length of string
        int count = test.length();
        
        while (n < count) {
            
            //When we encounter '.', we split the string and convert it into intger
            if(line[n] == '.') {
                i = atoi(s.c_str());
                
                store[m]=i;
                
                s ="";
                m++;
                n++;
                continue;
            }
            s = s + line[n];
            n++;
            
        }
        //convert remaining part and push it into vecotr
        i = atoi(s.c_str());
        store[m] = i;
        IPFile.close();
    }
    
    //convert ip numbers for GPS information
    this->CDN_addr = (16777216 * store[0]) + (65536 * store[1]) + (256 * store[2]) + store[3];

}

/*
 Using the numbers attained from above, we can get gps information from CSV file which contain USA local latitude and longitude information.
 */

pair <double, double> CDN_Node::get_gps_info (){

    ifstream f;
    string str;
    CData data;
    vector<CData> vdata;
    
    //this path have to change.
    f.open("/Users/wonjaelee/desktop/USA_edit.csv");
    
    if (!f.is_open())
    {
        cout << "failed to open the file!" << endl;
    }
    
    f >> str; // omit the header
    
    while(!f.eof())
    {
        //f >> index >> ch;
        data.Load(f);
        if (f.eof()) {
            vdata.push_back(data);
            break;
        }
        vdata.push_back(data);
    }
    
    f.close();
    
    vector<CData>::iterator it;
    int n = 0;
    
    for (it=vdata.begin();it!=vdata.end();it++)
    {
        if(CDN_addr >= vdata.at(n).GetStart() && CDN_addr <= vdata.at(n).GetEnd()) {
            cdn_gps.first = vdata.at(n).GetLatitude();
            cdn_gps.second = vdata.at(n).GetLongitude();
            return cdn_gps;
        }
       n++;
    }
    
    return cdn_gps;
}


/*
	void CDN_Node::lock_the_storage(){
		is_locked = true;
	}

	void CDN_Node::unlock_the_storage(){
		is_locked = false;
	}
*/



