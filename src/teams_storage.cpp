#include "teams_storage.h"
#include <fstream>
#include <sstream>

teams_storage::teams_storage()
	: f_delimiter(", ")
{

}

bool teams_storage::load_from_file(const string& fname)
{
	std::ifstream fs(fname);
	if (fs.is_open())
	{
		string line;
		while (std::getline(fs, line))
		{
			single_team_alias_list_ptr alias_list = std::make_shared<single_team_alias_list>();
			size_t pos = line.find(f_delimiter);
			while (pos != string::npos)
			{
				string name = line.substr(0, pos);
				alias_list->push_back(name);
				line.erase(0, pos + f_delimiter.length());
				pos = line.find(f_delimiter);
			}
			alias_list->push_back(line);
			f_aliases.push_back(alias_list);
		}

		save_to_file(fname);
		return true;
	}
	return false;
}

bool teams_storage::save_to_file(const string& fname)
{
	std::ofstream fs(fname);
	if (fs.is_open())
	{
		for (auto it = f_aliases.begin(); it != f_aliases.end(); ++it)
		{
			auto alias_list = *it;
			std::stringstream ss;

			for (auto iter = alias_list->begin(); iter != alias_list->end(); ++iter)
			{
				if (iter != alias_list->begin())
					ss << f_delimiter;
				ss << *iter;
			}
			fs << ss.rdbuf() << std::endl;
		}

		return true;
	}
	return false;
}

bool teams_storage::create_from_file(const string& fname)
{
	if (!load_from_file(fname))
		return false;

	for (auto it = f_aliases.begin(); it != f_aliases.end(); ++it)
	{
		single_team_alias_list_ptr lst = (*it);
		for (auto jt = lst->begin(); jt != lst->end(); ++jt)
		{
			string name = *jt;
			if (!f_teams_alias_map[name])
				f_teams_alias_map[name] = lst;
		}
	}

	return true;
}

void teams_storage::finalize(const string& fname)
{
	save_to_file(fname);
}

bool teams_storage::is_same_team(const string& alias1, const string& alias2)
{
	if (alias1 == alias2)
		return true;

	single_team_alias_list_ptr lst = f_teams_alias_map[alias1];
	if (lst)
	{
		auto it = std::find(lst->begin(), lst->end(), alias2);
		return (it != lst->end());
	}

	return false;
}

void teams_storage::add_alias(const string& alias1, const string& alias2)
{
	if (f_teams_alias_map[alias1] && f_teams_alias_map[alias2])
		return;

	if (f_teams_alias_map[alias1])
	{
		single_team_alias_list_ptr lst = f_teams_alias_map[alias1];
		if (std::find(lst->begin(), lst->end(), alias2) != lst->end())
			lst->push_back(alias2);

		f_teams_alias_map[alias2] = lst;
	}
	else if (f_teams_alias_map[alias2])
	{
		single_team_alias_list_ptr lst = f_teams_alias_map[alias2];
		if (std::find(lst->begin(), lst->end(), alias1) != lst->end())
			lst->push_back(alias1);

		f_teams_alias_map[alias1] = lst;
	}
	else
	{
		single_team_alias_list_ptr lst = std::make_shared<single_team_alias_list>();
		lst->push_back(alias1);
		lst->push_back(alias2);
		f_aliases.push_back(lst);

		f_teams_alias_map[alias1] = lst;
		f_teams_alias_map[alias2] = lst;
	}
}
