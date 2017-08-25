#ifndef __GNUC__
#pragma once
#endif
#ifndef __XR_INI_EXT_H__
#define __XR_INI_EXT_H__


#include "xr_ini_file.h"
#include <string>
using namespace xray_re;
using namespace std;

class xr_ini_ext : public xr_ini_file
{
	void create_section(const char* sect_name);
	ini_section* xr_ini_ext::section_by_name(string sname);

public:
	void parse(const char* buf, int num);
	bool section_exist(const char* sect_name);
	void add_item(const char* sect_name, string key, string value = "");
	bool set_item_value(const char* sect_name, string key, string value);
	string r_line(string sname, string kname);
	string tostring();
	vector<string> keys(string);
// 	bool save();
// 	bool load();
};

#endif // !