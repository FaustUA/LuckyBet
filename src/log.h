#pragma once

#include <iostream>
#include <fstream>

class my_log
{
public:
	my_log() {}

	static std::ofstream& stream;
};

std::ostream& final_log();
std::ostream& console_log();
std::istream& iconsole_log();