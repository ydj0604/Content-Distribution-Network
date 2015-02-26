#include "../CDN/CDNClientReceiver.h"
#include "cpprest/http_client.h"
#include "cpprest/asyncrt_utils.h"
#include "cpprest/rawptrstream.h"
#include "cpprest/containerstream.h"
#include "cpprest/producerconsumerstream.h"
#include "cpprest/filestream.h"
#include <string>

using namespace http::client;
using namespace std;
using namespace web;
using namespace utility;
using namespace concurrency;

template<typename _CharType>
pplx::task<Concurrency::streams::basic_ostream<_CharType>> OPENSTR_W(const utility::string_t &name, std::ios_base::openmode mode = std::ios_base::out)
{
	#if !defined(__cplusplus_winrt)
		return Concurrency::streams::file_stream<_CharType>::open_ostream(name, mode);
	#else
		auto file = pplx::create_task(
			KnownFolders::DocumentsLibrary->CreateFileAsync(
				ref new Platform::String(name.c_str()), CreationCollisionOption::ReplaceExisting)).get();

		return Concurrency::streams::file_stream<_CharType>::open_ostream(file, mode);
	#endif
}

void sendGetRequest(string fileName) {
	utility::string_t address = U("http://localhost:5000/cdn/cache");
	http_client client(address);

	auto fstream = OPENSTR_W<uint8_t>(U(fileName)).get();
	http_request msg(methods::POST);
    msg.set_response_stream(fstream);
    //fstream.close(std::make_exception_ptr(std::exception())).wait();
    http_response resp = client.request(msg).get();
    resp.content_ready().get();
}

void simpleTest() {
	utility::string_t port = U("5000");
	utility::string_t address = U("http://localhost:");
	address.append(port);
	CDNClientReceiver::initialize(address, NULL);
	std::cout << "type filename to download or Q to stop" << std::endl;
	std::string line;
	while(getline(std::cin, line)) {
		if(line=="Q")
			break;
		else
			sendGetRequest(line);
	}
	CDNClientReceiver::shutDown();
}

int main() {
	simpleTest();
}
