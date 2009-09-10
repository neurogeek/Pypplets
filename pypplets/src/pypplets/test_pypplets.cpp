#include <iostream>
#include <fstream>
#include "string"
#include "pypplets.h"

using namespace std;

string load_script(char *file)
{
    string data, line;
    ifstream script_f(file);

    if(script_f.is_open())
    {
        while(! script_f.eof() )
        {
            getline(script_f, line);
            data.append(line.append("\n"));
        }
        script_f.close();
    }
    return data;
}

int main(int argc, char *argv[])
{
    string script;
    script = load_script(argv[1]);
    Pypplets *eng = Pypplets_Init();
    eng->SetScript((char *) script.c_str());
    eng->TestRunCode();
    Pypplets_Destroy();
    return 0;
}
