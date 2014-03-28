// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "server.h"
extern "C"
{
  #include "webcurl.h"
}
#include "cacheTest.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <string.h>
#include <ctime>
#include <cstdio>
#include "lrucache.h"
// #include "fifocache.h"
// #include "random.h"

#define cacheSize 0

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::serverns;

class serverHandler : virtual public serverIf
{
public:
	serverHandler()
	{
    // Your initialization goes here
	}

	void getHtml(std::string& _return, const std::string& url)
	{
    // Your implementation goes here
		static CacheTest ct;
		static Cache<char *, char *> cache(cacheSize);
		//cout << "SIZE = " << SIZE << "\n";
		char * tempUrl = (char *)url.c_str();
		std::clock_t start, stop;

		if(cacheSize != 0)
		{
			if(!cache.search_cache(tempUrl))
			{
				cout << "Entry not in cache" << "\n";
				// Increment miss
				ct.updateMisses();
				/* Calculate tAvg */
				// Start clock
				start = std::clock();
				char * webPage = getWebPage(tempUrl);
				std::string body(webPage);
				cache.insert_into_cache(tempUrl, webPage);
				// Stop clock
				stop = std::clock();
				// Calculate tMem
				ct.updateTMem(start, stop);
				// Print test results
				cout << "Cumulative hit ratio = " << ct.calcHitRatio() << "\n";
				cout << "Cumulative average memory access time = " << ct.calcTAvg() << " secs\n\n";
				_return.assign(body);
			}
			else
			{
				cout << "Entry in cache" << "\n";
				// Increment hits
				ct.updateHits();
				/* Calculate tAvg */
				// Start clock
				start = std::clock();
				std::string body(cache.search_cache(tempUrl));
				// Stop clock
				stop = std::clock();
				// Calculate tCache
				ct.updateTCache(start, stop);
				// Print test results
				cout << "Cumulative hit ratio = " << ct.calcHitRatio() << "\n";
				cout << "Cumulative average memory access time = " << ct.calcTAvg() << " secs\n\n";
				_return.assign(body);
			}
		}
		else
		{
			// Increment miss */
			ct.updateMisses();
			/* Calculate tAvg */
			// Start clock
			start = std::clock();
			char * webPage = getWebPage(tempUrl);
			// Stop clock
			stop = std::clock();
			// Calculate tMem
			// m->tMem = m->tMem + ((stop - start) / (double) CLOCKS_PER_SEC);
			ct.updateTMem(start, stop);
			// Print test results
			cout << "Cumulative hit ratio = " << ct.calcHitRatio() << "\n";
			cout << "Cumulative average memory access time = " << ct.calcTAvg() << " secs\n\n";
			std::string body(webPage);
			_return.assign(body);
		}
	}
};

int main(int argc, char **argv)
{
	int port = 9090;
	shared_ptr<serverHandler> handler(new serverHandler());
	shared_ptr<TProcessor> processor(new serverProcessor(handler));
	shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
	shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

	TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
	server.serve();

	return 0;
}

