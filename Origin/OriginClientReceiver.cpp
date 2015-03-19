#include "cpprest/http_listener.h"
#include "OriginClientReceiver.h"
#include <iostream>
#include "OriginServer.h"
#include "../Shared.h"
#include <unordered_map>

using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace web::http::experimental::listener;
using namespace json;

OriginClientReceiver* OriginClientReceiver::m_instance = NULL;

OriginClientReceiver::OriginClientReceiver(utility::string_t url_explicit, utility::string_t url_sync) : m_listener_explicit(url_explicit), m_listener_sync(url_sync) {
	m_listener_explicit.support(methods::POST, std::bind(&OriginClientReceiver::handle_explicit, this, std::placeholders::_1));
	m_listener_sync.support(methods::POST, std::bind(&OriginClientReceiver::handle_sync, this, std::placeholders::_1));
}

void OriginClientReceiver::initialize(const string_t& address, OriginServer* origin) {
	OriginClientReceiver* instance = OriginClientReceiver::getInstance();
	uri_builder uri_explicit(address);
	uri_explicit.append_path(U("origin/explicit"));
	uri_builder uri_sync(address);
	uri_sync.append_path(U("origin/sync"));
	instance = new OriginClientReceiver(uri_explicit.to_uri().to_string(), uri_sync.to_uri().to_string());
	instance->setOrigin(origin);
	instance->open_explicit().wait();
	instance->open_sync().wait();
	ucout << utility::string_t(U("OriginClientReceiver is listening for client up/down requests at: ")) << uri_explicit.to_uri().to_string() << std::endl;
	ucout << utility::string_t(U("OriginClientReceiver is listening for client sync requests at: ")) << uri_sync.to_uri().to_string() << std::endl;
}

void OriginClientReceiver::shutDown() {
	OriginClientReceiver* instance = OriginClientReceiver::getInstance();
	if(instance==NULL)
		return;
	instance->close_explicit().wait();
	instance->close_sync().wait();
	delete instance;
}

void OriginClientReceiver::handle_sync(http_request message) {
	/* JSON Format
	Request
	{
		"FileList": [{"Name": "a.txt" ,"Hash": "ahash", "TimeStamp": "123123"}, {"Name": "b.txt" ,"Hash": "bhash", "TimeStamp": "123123"}], //list of client's local files and hashes and time stamps
		"IP": "1.1.1.1", //the sender client's ip address
		"Lat": 23.00, //the sender client's location
		"Lng": 148.12
	}

	Response
	{
		"FileList": [{"Name": "a.txt" ,"Address": "1.1.1.1", "Type": "UP"}, {"Name": "b.txt" ,"Address": "2.2.2.2", "Type": "DOWN", "TimeStamp": "123123123"}]
	}

	!! Note that TimeStamp is returned only for files to download

	*/

	try {
		if(message.headers().content_type()==U("application/json")) {
			//retrieve json object
			json::value jsonObj = message.extract_json().get();
			Address clientAddr(make_pair(jsonObj.at(U("Lat")).as_double(), jsonObj.at(U("Lng")).as_double()), utility::conversions::to_utf8string(jsonObj.at(U("IP")).as_string()));
			json::value& list = jsonObj.at(U("FileList"));

			//store file list into useful data structures
			unordered_map<string, string> clientFileNameToHashMap;
			vector< pair<string, string> > clientListTS; // <filename, file timestamp>
			for(auto& fileObj : list.as_array()) {
				clientListTS.push_back(make_pair(fileObj.at(U("Name")).as_string(), fileObj.at(U("TimeStamp")).as_string()));
				clientFileNameToHashMap[fileObj.at(U("Name")).as_string()] = fileObj.at(U("Hash")).as_string();
			}

			//get which files to upload and which files to download
			vector<string> uploadFileList, downloadFileList;
			unordered_map<string, string> nameToTsMap;
			if(m_origin->getListForSync(clientListTS, uploadFileList, downloadFileList, nameToTsMap)!=0) {
				message.reply(status_codes::NotFound, U("failure to get the list for sync"));
				return;
			}

			//construct vectors to be used vec<<file name, file hash>>
			vector< pair<string, string> > upVec, downVec;
			for(int i=0; i<uploadFileList.size(); i++) {
				upVec.push_back(make_pair(uploadFileList[i], clientFileNameToHashMap[uploadFileList[i]]));
			}
			for(int i=0; i<downloadFileList.size(); i++) {
				downVec.push_back(make_pair(downloadFileList[i], clientFileNameToHashMap[downloadFileList[i]]));
			}

			//get the address for each file to upload/download
			vector< pair<string, Address> > resultUploadList, resultDownloadList; //<file name, address>
			resultUploadList = m_origin->getListOfFilesUpload(upVec, clientAddr, true);
			resultDownloadList = m_origin->getListOfFilesDownload(downVec, clientAddr, true);

			//prepare response
			json::value respList = json::value::array();
			for(int i=0; i<resultUploadList.size(); i++) {
				json::value currFileObj = json::value::object();
				currFileObj[U("Name")] = json::value::string(U(resultUploadList[i].first));
				currFileObj[U("Address")] = json::value::string(U(resultUploadList[i].second.ipAddr));
				currFileObj[U("Type")] = json::value::string(U("UP"));
				respList[i] = currFileObj;
			}
			for(int i=0; i<resultDownloadList.size(); i++) {
				json::value currFileObj = json::value::object();
				currFileObj[U("Name")] = json::value::string(U(resultDownloadList[i].first));
				currFileObj[U("Address")] = json::value::string(U(resultDownloadList[i].second.ipAddr));
				currFileObj[U("Type")] = json::value::string(U("DOWN"));
				currFileObj[U("TimeStamp")] = json::value::string(U(nameToTsMap[resultDownloadList[i].first]));
				respList[resultUploadList.size()+i] = currFileObj;
			}
			json::value respFinal = json::value::object();
			respFinal[U("FileList")] = respList;

			//return the response
			message.reply(status_codes::OK, respFinal);
		} else {
			message.reply(status_codes::Forbidden, U("Json object is required"));
		}
	} catch(json::json_exception &e) {
		message.reply(status_codes::Forbidden, U("Invalid json object"));
		return;
	}

}

void OriginClientReceiver::handle_explicit(http_request message) {
	/* JSON Format
	Request
	{
		"Type": 0, //0=syncup 1=syncdown
		"FileList": [{"Name": "a.txt" ,"Hash": "ahash"}, {"Name": "b.txt" ,"Hash": "bhash"}], //list of client's local files and hashes
		"IP": "1.1.1.1", //the sender client's ip address
		"Lat": 23.00, //the sender client's location
		"Lng": 148.12
	}

	Response
	{
		"Type": 0, //0=syncup 1=syncdown
		"FileList": [{"Name": "a.txt" ,"Address": "1.1.1.1"}, {"Name": "b.txt" ,"Address": "2.2.2.2"}]
	}
	*/

	try {
		if(message.headers().content_type()==U("application/json")) {
			json::value jsonObj = message.extract_json().get();
			Address clientAddr(make_pair(jsonObj.at(U("Lat")).as_double(), jsonObj.at(U("Lng")).as_double()), utility::conversions::to_utf8string(jsonObj.at(U("IP")).as_string()));
			vector< pair<string, string> > clientList;
			json::value& list = jsonObj.at(U("FileList"));
			for(auto& fileObj : list.as_array())
				clientList.push_back(make_pair(utility::conversions::to_utf8string(fileObj.at(U("Name")).as_string()), utility::conversions::to_utf8string(fileObj.at(U("Hash")).as_string())));

			vector< pair<string, Address> > resultList;
			if(jsonObj.at(U("Type")).as_integer() == 0) { //sync up
				resultList = m_origin->getListOfFilesUpload(clientList, clientAddr);
			} else if(jsonObj.at(U("Type")).as_integer() == 1) { //sync down
				resultList = m_origin->getListOfFilesDownload(clientList, clientAddr);
			} else {
				message.reply(status_codes::Forbidden, U("Undefined Type"));
				return;
			}
			json::value respList = json::value::array();
			for(int i=0; i<resultList.size(); i++) {
				json::value currFileObj = json::value::object();
				currFileObj[U("Name")] = json::value::string(U(resultList[i].first));
				currFileObj[U("Address")] = json::value::string(U(resultList[i].second.ipAddr));
				respList[i] = currFileObj;
			}
			json::value respFinal = json::value::object();
			respFinal[U("Type")] = json::value::number(jsonObj.at(U("Type")).as_integer());
			respFinal[U("FileList")] = respList;
			message.reply(status_codes::OK, respFinal);

		} else {
			message.reply(status_codes::Forbidden, U("Json object is required"));
		}
	} catch(json::json_exception &e) {
		message.reply(status_codes::Forbidden, U("Invalid json object"));
		return;
	}
}
