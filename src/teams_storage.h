#pragma once

#include <string>
#include <list>
#include <map>
#include <memory>

using std::list;
using std::string;
using std::map;

typedef list<string> single_team_alias_list;
typedef std::shared_ptr<single_team_alias_list> single_team_alias_list_ptr;

class teams_storage
{
public:
	teams_storage();

	bool load_from_file(const string& fname);
	bool save_to_file(const string& fname);

	bool create_from_file(const string& fname);
	void finalize(const string& fname);

	bool is_same_team(const string& alias1, const string& alias2);
	void add_alias(const string& alias1, const string& alias2);
private:
	list<single_team_alias_list_ptr> f_aliases;
	map<string, single_team_alias_list_ptr> f_teams_alias_map;

	string f_delimiter;
};