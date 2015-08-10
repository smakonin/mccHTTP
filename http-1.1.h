/**
* This file is: 
*
* Copyright (C) 1999, 2002 Stephen Makonin.
* All Rights Reserved.
*
*/

extern "C++"
{
	string http_error_text(int error_code);
	int get_site_contents(map<string, string> *site, string url, int max_level, string save_in, string only_of_mime, string proxy_server, int proxy_port);
	int get_file_contents(string *data, string url, string save_as, string proxy_server, int proxy_port);
}