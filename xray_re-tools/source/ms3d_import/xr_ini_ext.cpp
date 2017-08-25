#include "xr_ini_ext.h"
#include <string>
using namespace xray_re;
using namespace std;

bool xr_ini_ext::section_exist(const char* sect_name)
{
	string tmp = string(sect_name);
	for (auto it = m_sections.begin(); it != m_sections.end(); ++it)
	{
		if ((*it)->name == tmp) return true;
	}
	return false;
}
void xr_ini_ext::create_section(const char* sect_name)
{
	ini_section* section = new ini_section(sect_name);
	m_sections.push_back(section);
}

void xr_ini_ext::add_item(const char* sect_name, string key, string value)
{
	if (!section_exist(sect_name)) create_section(sect_name);
	string tmp = string(sect_name);
	for (auto it = m_sections.begin(); it != m_sections.end(); ++it)
	{
		if ((*it)->name == tmp)
		{
			xr_ini_file::ini_item_vec& _it = (*it)->items;
			ini_item* itm = new ini_item(key);
			itm->value = value;
			_it.push_back(itm);
			return;
		};
	}
}

string xr_ini_ext::tostring()
{
	string res = "";
	for (auto it = m_sections.begin(); it != m_sections.end(); ++it)
	{
		res.append("[" + (*it)->name + "]\r\n");
		xr_ini_file::ini_item_vec _it = (*it)->items;
		for (auto it2 = _it.begin(); it2 != _it.end(); ++it2)
		{
			if ((*it2)->value.size())
				res.append((*it2)->name + " = " + (*it2)->value + "\r\n");
			else
				res.append((*it2)->name + "\r\n");
		}
		res.append("\r\n");
	}
	return res;
}

void xr_ini_ext::parse(const char* buf, int num)
{
	xr_ini_file::parse(buf, buf + num, "none");
}

string xr_ini_ext::r_line(string sname, string kname)
{
	xr_ini_file::ini_section* s = section_by_name(sname);
	for (auto it = s->items.begin(); it != s->items.end(); ++it)
	{
		if ((*it)->name == kname) return (*it)->value;
	}
	return "default";
}

xr_ini_file::ini_section* xr_ini_ext::section_by_name(string sname)
{
	for (auto it = m_sections.begin(); it != m_sections.end(); ++it)
		if ((*it)->name == sname) return (*it);
	create_section(sname.c_str());
	return m_sections.back();
}

vector<string> xr_ini_ext::keys(string sname)
{
	vector<string> res;
	xr_ini_file::ini_section* s = section_by_name(sname);
	for (auto it = s->items.begin(); it != s->items.end(); ++it) res.push_back((*it)->name);
	return res;
}
/*
#include "xr_file_system.h"
bool xr_ini_ext::save()
{
	string tmp = tostring();
	const char* m_buffer = tmp.c_str();
	xr_file_system& fs = xr_file_system::instance();
	string _path = "";
	if (!fs.resolve_path(PA_LOGS, "ms3d_tmp.txt", _path)) return false;
	xr_writer* w = fs.w_open(_path);
	if (w == 0)
		return false;
	w->w_raw(&m_buffer[0], tmp.size());
	fs.w_close(w);
	return true;
}

bool xr_ini_ext::load()
{
	string tmp = "";
	xr_file_system& fs = xr_file_system::instance();
	if(!fs.resolve_path(PA_LOGS, "ms3d_tmp.txt", tmp)) return false;
	if(!xr_ini_file::load(tmp)) return false;
	return true;
}
*/