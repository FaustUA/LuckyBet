#include "config.h"
#include <fstream>
#include <string>

config::config(const string& fname)
	: f_file_name(fname)
	, f_html_path("")
{

}

void config::load()
{
	std::ifstream fs(f_file_name);
	if (fs.is_open())
	{
		string line;
		while (std::getline(fs, line))
		{
			if (line[1] == '#')
				continue;

			const string c_delimiter = " = ";
			size_t pos = line.find(c_delimiter);
			if (pos == line.npos)
				continue;

			size_t pos1 = pos + c_delimiter.length();
			string key = line.substr(0, pos);
			string val = line.substr(pos1, line.length() - pos1 + 1);

			if (key == "html_path")
				f_html_path = val;
			else if (key == "parsed_path")
				f_parsed_path = val;
			else if (key == "teams_filename")
				f_teams_filename = val;
		}
	}
}

void config::save()
{

}

string config::get_html_path()
{
	return f_html_path;
}

string config::get_parsed_path()
{
	return f_parsed_path;
}

string config::get_teams_filename()
{
	return f_teams_filename;
}
