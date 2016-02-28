#include <boost/program_options.hpp>

#include <iostream>
#include <iterator>
using namespace std;
namespace po = boost::program_options;

// -h <ip> -p <port> -d <directory> 
po::variables_map getArgs(int ac, char* av[], string *host, int *port, string *directory) {
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            (",h", po::value<string>(host)->required(), "host ip")
            (",p", po::value<int>(port)->required(), "port")
            (",d", po::value<string>(directory)->required(), "working directory")
        ;

        po::variables_map vm;        
        po::store(po::parse_command_line(ac, av, desc), vm);

        if (vm.count("help")) {
            cout << desc << endl;
            exit(0);
        }

        po::notify(vm);    

        return vm;
    } catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        exit(1);
    } catch(...) {
        cerr << "Exception of unknown type!\n";
        exit(1);
    }
}

int main(int ac, char* av[]) {
    string host;
    string directory;
    int port;

    po::variables_map vm = getArgs(ac, av, &host, &port, &directory);
    HttpServer(host, port, directory).start();
}
