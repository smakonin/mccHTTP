/**
* This file is: 
*
* Copyright (C) 2002 Makonin Consulting Corp.
* All Rights Reserved.
*
* Written by: Stephen Makonin
*/

#pragma warning (disable : 4786)
#pragma warning (disable : 4503)

#include <iostream>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

#include "http-1.1.h"

#include <string.h>

#define VB_TRUE  1
#define VB_FALSE 0

const char *_stdcall open_url(char *error, char *url, char *save_as, char *proxy_address, long proxy_port)
{
	string t_url;
	string t_save_as;
	string data;
	string proxy_server;
	int error_code;
	char *m_data;


	t_url = url;
	if(!save_as)
		t_save_as = "";
	else
		t_save_as = save_as;
	if(!proxy_address)
		proxy_server = "";
	else
		proxy_server = proxy_address;
	strset(error, 0);

	error_code = get_file_contents(&data, t_url, t_save_as, proxy_server, proxy_port);

	if(error_code)
	{
		strcpy(error, http_error_text(error_code).c_str());
		return 0;
	}

	if(save_as)
		data = t_save_as;

	m_data = new char[data.length() + 1];
	strcpy(m_data, data.c_str());
	data = "";
	return m_data;
}

const char *_stdcall open_website(char *error, char *url, char *save_in, long level, char *only_of_type, char *proxy_address, long proxy_port)
{
	map<string, string> site;
	string t_url;
	string t_save_in;
	string only_type;
	string data;
	string proxy_server;
	string line;
	int error_code;
	char *m_data;

	t_url = url;
	if(!save_in)
		t_save_in = "";
	else
		t_save_in = save_in;

	if(t_save_in != "")
	{
		reverse(t_save_in.begin(), t_save_in.end());

		if(t_save_in.substr(0, 1) != "\\")
			t_save_in = "\\" + t_save_in;

		reverse(t_save_in.begin(), t_save_in.end());
	}
	
	
	if(!only_of_type)
		only_type = "";
	else
		only_type = only_of_type;
	if(!proxy_address)
		proxy_server = "";
	else
		proxy_server = proxy_address;
	strset(error, 0);
	data = "";

	error_code = get_site_contents(&site, t_url, level, t_save_in, only_type, proxy_server, proxy_port);

	if(error_code)
	{
		strcpy(error, http_error_text(error_code).c_str());
		return 0;
	}

	map<string, string>::iterator p;
	map<string, string>::iterator h;
	h = site.begin();
	p = h;
	p++;
	while(p != site.end())	
	{
		if(t_save_in == "")
		{
			line = "http://" + h->second + p->first + "\t" + p->second + "\r\n";
			replace(line.begin(), line.end(), '\\', '/');
		}
		else
		{
			line = t_save_in + h->second + p->first + "\t" + p->second + "\r\n";
			replace(line.begin(), line.end(), '/', '\\');
		}

		data += line;

		p++;
	}

	site.clear();
	m_data = new char[data.length() + 1];
	strcpy(m_data, data.c_str());
	data = "";
	return m_data;
}




