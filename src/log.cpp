#include "log.h"

std::ostream& final_log()
{
	//return std::cout;

	static std::ofstream f("ggg.txt");
	return f;
}

std::ostream& console_log()
{
	return std::cout;
}

std::istream& iconsole_log()
{
	return std::cin;
}