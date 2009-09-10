#include <iostream>
#include <Python.h>
#include "pypplets.h"

/*
 * -----------        ------------        ----------
 * | Plugin  | <----> | Pypplets | <----> | Script |
 * -----------        ------------        ----------
*/

//Pypplets Class Methods
void Pypplets::SetScript(std::string *script){
	isset = true;
	p_script = script;
	return;
}

std::string *Pypplets::GetScript(){
	return p_script;
}
/***
 * Pypplets::TestRunCode
 *   Runs the script (Test)
 */
void Pypplets::TestRunCode()
{
    int status;
    status = PyRun_SimpleString(p_script->c_str());
    std::cout << "Status: " << status << std::endl;
    return;
}

//End Pypplets Class Methods

//Wrappers
//Init Pypplets
Pypplets *Pypplets_Init(){
	Py_Initialize();
	Pypplets *engine = new Pypplets();
	return engine;
}

NPError Pypplets_Destroy(){
	Py_Finalize();
	return NPERR_NO_ERROR;
}
