#pragma once

#include <string>

using std::string;

class config
{
public:
	config(const string& fname = "");

	void load();
	void save();

	string get_html_path();
	string get_parsed_path();
	string get_teams_filename();
private:
	string f_file_name;

	string f_html_path;
	string f_parsed_path;
	string f_teams_filename;
};