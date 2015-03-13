#include "CDN_Node.h"
#include "CDNReceiver.h"
#include "CDNSender.h"
#include "csv.h" //use CSV parser
#include "CDN_Cache.h"
#include "Shared.h"
#include <string>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

/*
 	Address la_client(make_pair(34.05, -118.44), "0.0.0.0");
	Address sf(make_pair(37.77, -122.42), "1.1.1.1");
	Address seattle(make_pair(47.61, -122.33), "2.2.2.2");
	Address bahama(make_pair(25.03, -77.40), "3.3.3.3");
	Address northkorea(make_pair(40.34, 127.51), "4.4.4.4");
	Address austin_fss(make_pair(30.27, -97.74), "255.255.255.255");
 */


CDN_Node::CDN_Node(string cdnIpAddr, string metaIpAddr, string fssIpAddr, string city) {
	make_storage();
	m_metaIpAddr = metaIpAddr;
	m_fssIpAddr = fssIpAddr;
	m_sender = new CDNSender("http://"+metaIpAddr, "http://"+fssIpAddr);
	m_sender->setCDN(this);

	if(city=="la") {
		m_address.ipAddr = cdnIpAddr;
		m_address.latLng = make_pair(34.05, -118.44);
	} else if(city=="sf") {
		m_address.ipAddr = cdnIpAddr;
		m_address.latLng = make_pair(37.77, -122.42);
	} else if(city=="st") {
		m_address.ipAddr = cdnIpAddr;
		m_address.latLng = make_pair(47.61, -122.33);
	} else if(city=="bh") {
		m_address.ipAddr = cdnIpAddr;
		m_address.latLng = make_pair(25.03, -77.40);
	} else if(city=="nk") {
		m_address.ipAddr = cdnIpAddr;
		m_address.latLng = make_pair(40.34, 127.51);
	} else if(city=="au") {
		m_address.ipAddr = cdnIpAddr;
		m_address.latLng = make_pair(30.27, -97.74);
	} else {
		get_address(); // initialize cdn address !!!!!! NEED TO CHANGE
	}

	this->get_size_of_storage();

    m_cdnId = -1;
    m_sender->sendRegisterMsgToMeta(m_address, m_cdnId);
    cout<<"Registered ID: "<<m_cdnId<<endl;

    cout<<"CDN Lat: "<<m_address.latLng.first<<" ";
    cout<<"CDN Lng: "<<m_address.latLng.second<<endl;
    cout<<"CDN Addr: " + m_address.ipAddr<<endl;
    cout<<"Meta Addr: " + metaIpAddr<<endl;
    cout<<"FSS Addr: " + fssIpAddr<<endl;
}

CDN_Node::~CDN_Node() {
	delete m_sender;
}

void CDN_Node::startListening(){
	CDNReceiver::initialize(U("http://"+m_address.ipAddr), this);
}

void CDN_Node::endListening(){
	CDNReceiver::shutDown();
}

bool CDN_Node::make_storage() {
	strcpy(wd, CDN_DIR);
	mkdir(wd, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	return true;
}

void CDN_Node::get_address() {
    get_and_set_CDN_addr(); //ip address is assigned in get_and_set_CDN_addr()
    get_gps_info();
    
    m_address.latLng.first = cdn_gps.first;
    m_address.latLng.second = cdn_gps.second;
}

//TODO: take relative file path instead of file name as the first argument !!
bool CDN_Node::look_up_and_remove_storage(string filename, int signal) {
		/*
			Given filename, and signal is 0, look up the storage of Node and return true if there is match.
		
            If signal is 1, look up the file and remove it from the directory.
         */
    
            //const char* cstr = filename.c_str();
            string filepath = CDN_DIR + filename;
            //if file is not exists, return false;
            //if file is exists and signal is 0, return true
            //if file is exists and signal is 1, remove it and return true
            if(!ifstream(filepath)) {
                cout << "Can't find the file" << endl;
                return false;
            }  else if(ifstream(filepath) && signal ==0) {
                return true;
            } else{
                if(remove(filepath.c_str()) == 0) {
                    return true;
                } else {
                    return false;
                }
            }
        /*
			if((dir = opendir(wd)) != NULL) {
				while ((ent = readdir(dir)) != NULL) {
					if (strcmp(ent->d_name,cstr)==0 && signal == 0) {
						return true;
                    } else if (strcmp(ent->d_name,cstr)==0 && signal == 1) {
                        //remove the file
                        char temp[256];
                        strcpy(temp,path_maker(cstr));
                        if(remove(temp) == 0) {
                            return true;
                        } else {
                            return false;
                        }
                    }
				}
				closedir (dir);
			} else {
				perror("Can't open the working directory");
				return false;
			}
         */
			return false;
}

bool CDN_Node::write_file(const string &contents, string filename, vector<string>& deletedfiles){
    
    //Configure the storage's capacity and free some portion if exceeds and insert the new file information
    long long file_size = contents.size();
    
    if(!managing_files(filename, file_size, deletedfiles)) {
    	return false;
    }
    /*
    //convert string to char
    const char* cstr = filename.c_str();
    char temp[256];
    strcpy(temp,path_maker(cstr));
    */
    string filepath = CDN_DIR + filename;
    
    //create parent directories
    string cmd = "mkdir -p $(dirname " + filepath + ")";
    int ret = system(cmd.c_str());

    //Write to file
    ofstream f(filepath.c_str());
    if(f.is_open()){
        f << contents;
        f.close();
    } else {
        return false;
    }
    return true;
}

string CDN_Node::load_file(string filename) {
    //convert string to char
    
    /*
    const char* cstr = filename.c_str();
    char temp[256];
    strcpy(temp,path_maker(cstr));
    //Return contents of file
    ifstream f (temp);
     */
    string filepath = CDN_DIR + filename;
    ifstream f(filepath.c_str());
    
    if(f.is_open()) {
        //update cache information
        file_tracker.get(filename);
        stringstream buf;
        buf << f.rdbuf();
        return buf.str();
    } else {
        return "Can't load the file";
    }
}

/*
 Whenever we try to save the file into Directory, CDN takes the new file size and compare it whether
 the file size fits to the capacity or not. If it exceeds the limit, this function remove least used
 files until it satisfy the condition.
 */
bool CDN_Node::managing_files(string filename,long long file_size, vector<string>& deletedfiles) {
    while((this->get_size_of_storage() + file_size) > storage_capacity) {
        string file_name = file_tracker.remove(deletedfiles);
        if(file_name=="")
        	return false;
        this->look_up_and_remove_storage(file_name, 1);
    }
    //insert new file and size into tracker
    file_tracker.set(filename, file_size);
    return true;
}

bool CDN_Node::delete_file(string filename){
	file_tracker.remove(filename);
	return this->look_up_and_remove_storage(filename,1);
}

long long CDN_Node::get_size_of_storage() {
    size_of_storage = 0;
    struct stat sb;
    /*
    if((dir = opendir(wd)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            stat(path_maker(ent->d_name), &sb);
            size_of_storage += (long long)sb.st_size;
        }
        closedir (dir);
    } else {
                perror("Can't open the directory");
        return EXIT_FAILURE;
    }*/
    if((dir = opendir(CDN_DIR)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if( stat(path_maker(ent->d_name),&sb) == 0 )
            {
            
                if( sb.st_mode & S_IFDIR && (string)ent->d_name != "." && (string)ent->d_name != ".." )
                {
                    //it's a directory
                    //cout << "directory: " << ent->d_name << endl;
                    if((dir_2 = opendir(path_maker(ent->d_name))) ==NULL) {
                        cout << "this directory can't be opened" << endl;
                            break;
                    }
                    while((ent_2 = readdir(dir_2))!= NULL) {
                        //cout << "ent_2: " << ent_2->d_name << endl;
                        stat(path_maker(ent_2->d_name), &sb);
                        size_of_storage += (long long)sb.st_size;
                    }
                    closedir(dir_2);
                }
                else
                {
                    //it's a file
                    //cout << "file: " << ent->d_name << endl;
                    size_of_storage += (long long)sb.st_size;
                }
                
            }
        }
        closedir(dir);
        }
    else
    {
        //error
         cout << "Can't read directory" << endl;
    }
                
        return size_of_storage;
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
        m_address.ipAddr = line; //assign ip address
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
    f.open("/home/jin/workspace/C++/CDN/CDN/USA_edit.csv");
    
    if (!f.is_open())
    {
        cout << "failed to open the file for gps info" << endl;
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

char* CDN_Node::path_maker(const char* name){
	char path_file[256];
	strcpy(path_file, wd);
	strcat(path_file, "/");
	return strcat(path_file, name);
}


