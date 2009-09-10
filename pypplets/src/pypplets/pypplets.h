#include <npapi.h>
#include <Python.h>
#include <string>

class Pypplets
{
	private:
		std::string *p_script;
		bool isset;
		PyObject *gWindow;

	public:
		Pypplets(){}
		~Pypplets(){ delete p_script; }
		
		
		void SetScript(std::string *script);
		std::string *GetScript();
		
		void SetWindow(PyObject *gWindow){ gWindow = gWindow; }
        void TestRunCode();
};

Pypplets *Pypplets_Init();
NPError Pypplets_Destroy();
