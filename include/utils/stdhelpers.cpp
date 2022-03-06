#include "stdhelpers.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../utils/log.h"
std::string string_readf(const std::string& path) {
	int retry = 5;
	while (retry-- > 0)
	{
		try
		{
			std::ifstream input_file(path);
			if (!input_file.is_open()) {
				/*std::cerr << "Could not open the file - '"
					<< path << "'" << std::endl;*/
			}
			return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
		}
		catch(int err)
		{
			log("failed to read file, retry");
			Sleep(200);
		}
	};
	MessageBoxA(NULL, string_format("Failed to readfile '%s'", path.c_str()).c_str(), "Error", MB_ICONERROR);
	exit(EXIT_FAILURE);
}

