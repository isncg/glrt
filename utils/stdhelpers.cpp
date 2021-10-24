#include "stdhelpers.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
std::string string_readf(const std::string& path) {
	std::ifstream input_file(path);
	if (!input_file.is_open()) {
		/*std::cerr << "Could not open the file - '"
			<< path << "'" << std::endl;*/
		MessageBoxA(NULL, string_format("Failed to readfile '%s'", path.c_str()).c_str(), "Error", MB_ICONERROR);
		exit(EXIT_FAILURE);
	}
	return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

