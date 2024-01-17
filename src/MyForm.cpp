#include "MyForm.h"              //This application has a lot of auto generated code by Windows Form Application
#include<curl/curl.h>

using namespace System;                   //Auto Generated Code by Windows Form Application
using namespace System::Windows::Forms;       //Auto Generated Code by Windows Form Application

[STAThreadAttribute]

void main(array<String^>^ args) {
	curl_global_init(CURL_GLOBAL_ALL);            // Initialize libcurl
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	$safeprojectname$::MyForm form;
	Application::Run(% form);
	curl_global_cleanup();                // Clean up libcurl
}