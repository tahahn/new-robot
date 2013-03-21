#include "Subsystem.h"
#include <fstream>

using namespace std;
using namespace ramcore;

class Example : public Subsystem
{
    int i = 0;

    void onSetup()
    {
        setUpdateInterval(10);
    }

    void onUpdate()
    {
        i++;
    }

    void onShutdown()
    {
        Json::Value config = getConfiguration();
        string path = getTopDirectory();
        path += "/build/";
        path += config["filename"].asString();

        ofstream outputFile;
        outputFile.open(path);

        outputFile << "\nI ran for " << i/100.f << " seconds.\n";

        outputFile.close();
    }
};

int main(int argc, char *argv[])
{
    initSubprocess();
    runSubsystem(new Example());
}
