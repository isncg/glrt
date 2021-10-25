#include "log.h"
#include <Windows.h>
#include <iostream>
void log(const char* message, LogLevel level)
{
	std::cout << "LOG[" << level << "]" << message << std::endl;
	if (level == LogLevel::Info)
	{
		return;
	}

	UINT type = 0;
	const char* typedesc = NULL;
	if (level == LogLevel::Warning)
	{
		type = MB_ICONWARNING;
		typedesc = "Warning";
	}
	else if (level == LogLevel::Error)
	{
		type = MB_ICONERROR;
		typedesc = "Error";
	}
	MessageBoxA(NULL, message, typedesc, type);
}