#include "mmorpg.h"
#include "server.h"

using namespace std;

int main()
{
    try {
        GameServer server(1234);
        server.start();

        // Keep the server alive
        std::cout << "Server is running. Press Enter to exit..." << std::endl;
        std::cin.get();  // Wait for user input before exiting
    }
    catch (const exception& e) {
        cerr << "💀 Critical error: " << e.what() << endl;
        return 1;
    }
	return 0;
}
