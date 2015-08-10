/**
* This file is: 
*
* Copyright (C) 1999, 2002 Stephen Makonin.
* All Rights Reserved.
*
*/

#pragma warning (disable : 4786)
#pragma warning (disable : 4503)

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

#include <winsock.h>
#include <direct.h>
#include "http-1.1.h"


const bool is_debug_mode = false;
const char log_file[] = "c:/socket-log.txt";

const char hex_array[] = "0123456789ABCDEF";
const char http[] = "http://";
const char html_type[] = "text/html";
const char http11[] = "HTTP/1.1 ";
const char http_ok[] = "200";
const char http_move1[] = "301";
const char http_move2[] = "302";
const char http_location[] = "Location: ";
const char http_length[] = "Content-Length: ";
const char http_type[] = "Content-Type: ";
const long max_recv = 4096;
const long max_block_size = max_recv * 100;
const long max_timeout = 5;
const long max_filename_len = _MAX_PATH-5;

void log_data(string data)
{
	FILE *fp;

	fp = fopen(log_file, "ab+");

	fwrite(data.c_str(), data.length(), 1, fp);

	fclose(fp);
}

void log_data(vector<unsigned char> *data)
{
	FILE *fp;
	string byte;
	vector<unsigned char>::iterator p;

	fp = fopen(log_file, "ab+");

	p = data->begin();
	while(p != data->end())
	{
		fwrite(p, 1, 1, fp);
		p++;
	}

	fclose(fp);
}

long save2file(string filename, vector<unsigned char> *data)
{
	FILE *fp;
	string byte;
	vector<unsigned char>::iterator p;

	fp = fopen(filename.c_str(), "wb");

	if(fp)
	{
		p = data->begin();
		while(p != data->end())
		{
			if(fwrite(p, 1, 1, fp) < 1)
			{
				fclose(fp);
				return 334;
			}
			p++;
		}

		fclose(fp);
	}

	if(filename.length() > max_filename_len)
		return 334;

	return 0;
}

unsigned long hstoul(string hex_number)
{
	unsigned long num;
	long str_pos;
	long count;
	long multiplier;
	long max_len;

	num = 0;
	max_len = hex_number.length();
	transform(hex_number.begin(), hex_number.end(), hex_number.begin(), toupper);

	for(str_pos = 0; str_pos < max_len; str_pos++)
	{
		count = -1;
		while(hex_number.c_str()[str_pos] != hex_array[++count]);

		multiplier = (max_len - str_pos - 1) << 2;
		num += (count << multiplier);
	}

	return num;
}

unsigned long get_ip_address(string address)
{
	char ip_addr[33];
	char *ptr;
	unsigned long ip;
	unsigned char a1;
	unsigned char a2;
	unsigned char a3;
	unsigned char a4;
	unsigned long count;

	strcpy(ip_addr, address.c_str());
	for(count = 0; count < strlen(ip_addr); count++)
		if(!isdigit(ip_addr[count]) && ip_addr[count] != '.')
			return 0;

	ptr = strtok(ip_addr, ".");
	a1 = (unsigned char)atoi(ptr);
	ptr = strtok(NULL, ".");
	a2 = (unsigned char)atoi(ptr);
	ptr = strtok(NULL, ".");
	a3 = (unsigned char)atoi(ptr);
	ptr = strtok(NULL, ".");
	a4 = (unsigned char)atoi(ptr);	

	ip = (unsigned long)(a4 << 24) + (unsigned long)(a3 << 16) + (unsigned long)(a2 << 8) + a1;

	return ip;
}

void break_http_header(string data, string *type, string *size, bool *is_html, bool *has_moved)
{
	int i, j;
	string status;
	string code;

	*is_html = false;
	*has_moved = false;
	*size = "-1";

	i = data.find("\r\n") - sizeof(http11) + 1;
	status.assign(data, sizeof(http11) - 1, i);
	code.assign(data, sizeof(http11) - 1, 3);

	if(code == http_ok)
	{
		i = data.find(http_type);
		i += sizeof(http_type) - 1;
		j = data.find("\r\n", i) - i;
		*type = data.substr(i, j);

		if(*type == html_type)
			*is_html = true;

		if(i >= 0)
		{
			i = data.find(http_length);
			i += sizeof(http_length) - 1;
			j = data.find("\r\n", i) - i;
			if(data.substr(i, j) != "")
				*size = data.substr(i, j);
		}
	}
	else if(code == http_move1 || code == http_move2)
	{
		i = data.find(http_location);
		i += sizeof(http_location) - 1;
		j = data.find("\r\n", i) - i;
		*type = data.substr(i, j);

		*has_moved = true;
	}
	else
	{
		*type = status;
	}
}

int socket_connect(SOCKET *my_socket, string host, int port, int level)
{
	static SOCKADDR_IN in_address;
	WORD ver_request = MAKEWORD(1,1);
	WSADATA ws_data;
    LPHOSTENT host_entry;
	unsigned long addr;
	int return_code;
	int error_code;

	error_code = 0;
	switch(level)
	{
	case 1:
		return_code = WSAStartup(ver_request, &ws_data);
		if(ws_data.wVersion != ver_request)
		{
			error_code = WSAVERNOTSUPPORTED;
		}
		else
		{
			addr = get_ip_address(host);
			if(!addr)
			{
				host_entry = gethostbyname(host.c_str());
				if(!host_entry)
				{
					error_code = WSAGetLastError();
				}
				else
				{
					if(!addr)
					{
						in_address.sin_family = AF_INET;
						in_address.sin_addr = *((LPIN_ADDR)*host_entry->h_addr_list);
						in_address.sin_port = htons((u_short)port);
					}
					else
					{
						in_address.sin_family = AF_INET;
						in_address.sin_addr = *((LPIN_ADDR)&addr);
						in_address.sin_port = htons((u_short)port);
					}
				}
			}
		}
		break;

	case 2:
		*my_socket = socket(AF_INET, SOCK_STREAM, 0);
		if(*my_socket == INVALID_SOCKET)
		{
			error_code = WSAGetLastError();
		}
		else
		{
			return_code = connect(*my_socket, (LPSOCKADDR)&in_address, sizeof(struct sockaddr));
			if(return_code == SOCKET_ERROR)
				error_code = WSAGetLastError();
		}
		break;
		
	default:
		error_code = 101;
		break;
	}

	return error_code;
}

int socket_disconnect(SOCKET my_socket, int level)
{
	int error_code;

	error_code = 0;
	switch(level)
	{
	case 1:
		WSACleanup();
		break;

	case 2:
		shutdown(my_socket, 0x02);
		closesocket(my_socket);
		break;
		
	default:
		error_code = 101;
		break;
	}

	return error_code;
}

int socket_send(SOCKET my_socket, string data)
{
	int return_code;
	int error_code;

	error_code = 100;
	if(my_socket)
	{
		error_code = 0;

		return_code = send(my_socket, data.c_str(), data.length(), 0);
		if(return_code == SOCKET_ERROR)
			error_code = WSAGetLastError();
	}

	return error_code;
}

int socket_receive(SOCKET my_socket, vector<unsigned char> *data, string end_mark, int length, bool clear_data=true)
{
	string current_end;
	char bytes[max_recv + 1];
	int len, i, total_downloaded;
	int error_code;

	FD_SET fd = {1, my_socket};
	TIMEVAL tv = {max_timeout, 0};

	if(clear_data)
		data->clear();

	error_code = 0;
	total_downloaded = 0;
	len = 1;
	current_end.append(end_mark.size(), ' ');
	while(true) 
	{
		memset(bytes, 0, max_recv + 1);

		if(!total_downloaded && length > 0)
		{
			if(max_recv > length)
				len = length;
			else
				len = max_recv;
		}
		else if(length == -1)
		{
			len = 1;
		}
		else
		{
			len = length - total_downloaded;
			if(len > max_recv)
				len = max_recv;
		}

		if (len > 0) 
			if (!select(0, &fd, NULL, NULL, &tv))
				return error_code;

		len = recv(my_socket, bytes, len, 0);
		if(len == SOCKET_ERROR)
		{
			error_code = WSAGetLastError();
			break;
		}
		
		total_downloaded += len;

		if(len == 1)
		{
			data->push_back(bytes[0]);

			current_end += bytes[0];
			current_end.erase(0, 1);				
		}
		else if(len > 0)
		{
			for(i = 0; i < len; i++)
				data->push_back(bytes[i]);
		}

		if(end_mark == current_end && current_end != "")
			break;

		if((total_downloaded >= length && length > 0) || len == 0)
			break;
	}

	if(is_debug_mode)
		log_data(data);

	return error_code;
}

bool break_url(string url, string *host, string *location)
{
	int i;

	i = url.find(http);
	if(i != string::npos)
		url.erase(i, sizeof(http) - 1);

	i = url.find("/");
	if(i == string::npos)
	{
		*host = url;
		*location = "/";
	}
	else
	{
		host->assign(url, 0, i);
		location->assign(url, i, url.size());
	}

	return true;
}

string fix_path(string url, string location)
{
	int i, j;
	string new_path;

	i = location.find("#");
	if(i != string::npos)
		location.erase(i);

	i = location.find("?");
	if(i != string::npos)
		location.erase(i);

	if(location.substr(0, 1) == "/")
		return location;

	reverse(url.begin(), url.end());
	i = url.find("/") + 1;
	url.erase(0, i);
		
	while((i = location.find("../")) != string::npos)
	{
		j = url.find("/") + 1;

		location.erase(i, 3);
		url.erase(0, j);
	}
	reverse(url.begin(), url.end());

	new_path = url + "/" + location;
	return new_path;
}

string create_path(string save_in, string host, string url)
{
	string new_path;
	string part;
	string filename;
	char buf[1024];
	int i;
	char *ptr;

	if(save_in != "")
	{
		reverse(save_in.begin(), save_in.end());

		if(save_in.substr(0, 1) != "\\")
			save_in = "\\" + save_in;

		reverse(save_in.begin(), save_in.end());
	}

	new_path = save_in + host + url;
	reverse(new_path.begin(), new_path.end());
	i = new_path.find("/");
	if(i != string::npos)
		new_path.erase(0, i);
	filename = "";
	if(!i)
		filename = "index.htm";
	reverse(new_path.begin(), new_path.end());
	memset(buf, 0, sizeof(buf));
	memcpy(buf, new_path.c_str(), new_path.length());

	ptr = strtok(buf, "\\/");
	part = "";
	while(ptr)
	{
		part += ptr;
		_mkdir(part.c_str());
		ptr = strtok(NULL, "\\/");
		part += "/";
	}

	new_path = save_in + host + url + filename;
	return new_path;
}

bool is_of_same_mime(string only_of_mime, string file_mime)
{
	int i;

	i = file_mime.find(only_of_mime);
	if(i == string::npos)
		return false;

	return true;
}

int get_file_header(SOCKET my_socket, string host, string url, string *data)
{
	vector<unsigned char> raw_data;
	string command;
	int error_code;
	int i;

	*data = "";
	command = "HEAD " + url + " HTTP/1.1\r\nUser-Agent: x86 mccHTTP Engine (www.makonin.com)\r\nHost: " + host + "\r\n\r\n";

	error_code = socket_connect(&my_socket, host, 80, 2);

	if(!error_code)
		error_code = socket_send(my_socket, command);

	if(!error_code)
		error_code = socket_receive(my_socket, &raw_data, "\r\n\r\n", -1);

	socket_disconnect(my_socket, 2);

	for(i = 0; i < raw_data.size(); i++)
		*data += raw_data[i];

	return error_code;
}

int get_file(SOCKET my_socket, string host, string url, string *data, string save_as, bool save_only)
{
	vector<unsigned char> raw_data;
	vector<unsigned char> junk_data;
	string command;
	string header;
	string type;
	string size;
	int error_code;
	int i, bytes, lsize;
	bool is_html;
	bool has_moved;

	*data = "";
	command = "GET " + url + " HTTP/1.1\r\nUser-Agent: x86 mccHTTP Engine (www.makonin.com)\r\nHost: " + host + "\r\n\r\n";

	error_code = socket_connect(&my_socket, host, 80, 2);

	if(!error_code)
		error_code = socket_send(my_socket, command);

	if(!error_code)
	{
		error_code = socket_receive(my_socket, &raw_data, "\r\n\r\n", -1);
		if(!error_code)
		{
			for(i = 0; i < raw_data.size(); i++)
				header += raw_data[i];

			break_http_header(header, &type, &size, &is_html, &has_moved);
			lsize = atoi(size.c_str());
			if(lsize == -1)
			{
				bytes = 0;
				raw_data.clear();
				do
				{
					error_code = socket_receive(my_socket, &junk_data, "\r\n", -1);
					if(!error_code)
					{
						size = "";
						for(i = 0; i < junk_data.size(); i++)
						{
							if(isxdigit(junk_data[i]))
								size += junk_data[i];
						}
					
						bytes = hstoul(size) + 2;
						if(bytes > 2 && bytes < max_block_size)
							error_code = socket_receive(my_socket, &raw_data, "", bytes, false);
					}
				} while(!error_code && bytes > 2 && bytes < max_block_size);
			}
			else if(lsize > 0)
			{
				error_code = socket_receive(my_socket, &raw_data, "", lsize);
			}
		}
	}

	socket_disconnect(my_socket, 2);

	if(save_as != "")
		error_code = save2file(save_as, &raw_data);

	if(!save_only)
		for(i = 0; i < raw_data.size(); i++)
			*data += raw_data[i];

	raw_data.clear();
	junk_data.clear();

	return error_code;
}

void crawl(int *error_code, SOCKET my_socket, string host, string url, int lvl, int max_lvl, string save_in, string only_of_mime, map<string, string> *site)
{
	int i, j, pos, count;
	string data;
	string type;
	string tag;
	string ltag;
	string param;
	string size;
	string save_as;
	bool is_html;
	bool has_moved;
	bool equal_found;
	int quote_count;
	map<string, string>::iterator p;

	if(lvl > max_lvl && max_lvl != -1)
		return;

	if(url.length() >= max_filename_len) 
		return;

	i = 0;
	has_moved = true;
	while(has_moved && i < 5)
	{
		*error_code = get_file_header(my_socket, host, url, &data);
		if(*error_code)
				return;
		
		if(data.size() <= 0)
		{
			*error_code = WSAEACCES;
			return;
		}

		break_http_header(data, &type, &size, &is_html, &has_moved);
		tag = type + " (" + size + " bytes)";

		if(has_moved)
			break_url(type, &type, &url);

		i++;
	}

	p = site->find(url);
	if(p == site->end())
	{
		site->insert(pair<string, string>(url, tag));

		if(is_debug_mode)
			cout << url << " " << tag << endl;

		save_as = "";
		if(save_in != "")
			if(only_of_mime == "" || (only_of_mime != "" && is_of_same_mime(only_of_mime, type)))
				save_as = create_path(save_in, host, url);

		if(save_as == "ERROR")
		{
			*error_code = 103;
			return;
		}

		if(!is_html && save_as != "")
		{
			*error_code = get_file(my_socket, host, url, &data, save_as, true);
			if(*error_code)
				return;

			data = "";
		}
		
		if(is_html)
		{
			*error_code = get_file(my_socket, host, url, &data, save_as, false);
			if(*error_code)
				return;
			
			pos = 0;
			while(1)
			{
				i = data.find("<", pos);
				
				if(i == string::npos)
					break;

				j = data.find(">", i) - i + 1;
				pos = i + j;

				tag.assign(data, i, j);

				count = 0;
				while(1)
				{
					switch(count)
					{
					case 0:
						param = "src";
						break;
					case 1:
						param = "href";
						break;
					case 2:
						param = "action";
						break;
					case 3:
						param = "background";
						break;
					case 4:
						param = "usemap";
						break;
					case 5:
						param = "url";
						break;
					default:
						param = "";
						break;
					}

					if(param == "")
						break;

					ltag = tag;
					transform(ltag.begin(), ltag.end(), ltag.begin(), tolower);
					i = ltag.find(param);
					if(i != string::npos)
					{
						tag.erase(0, i + param.size());
						
						equal_found = false;
						quote_count = 0;
						while(tag.substr(0, 1) == " " || tag.substr(0, 1) == "=" || tag.substr(0, 1) == "\"")
						{
							if(tag.substr(0, 1) == "=")
								equal_found = true;
							
							if(tag.substr(0, 1) == "\"")
								quote_count++;

							tag.erase(0, 1);
						}

						if(quote_count == 1 && equal_found)
						{						
							i = 0;
							while(tag.substr(i, 1) != " " && tag.substr(i, 1) != "\"" && tag.substr(i, 1) != ">")
								i++;

							tag.erase(i);
							
							if(tag.find("://") == string::npos && tag.find("mailto:") == string::npos)
							{
								tag = fix_path(url, tag);
								p = site->find(tag);
								if(p == site->end())
									crawl(error_code, my_socket, host, tag, lvl + 1, max_lvl, save_in, only_of_mime, site);

								if(*error_code)
									break;
							}
						}
					}

					count++;
				}

				if(*error_code)
					break;
			}

			data = "";
		}
	}
}

string http_error_text(int error_code)
{
	string error_text;

	switch(error_code)
	{
	case 100:
		error_text = "Not connected to server.";
		break;
	case 101:
		error_text = "Invalid connect/disconnect level.";
		break;
	case 102:
		error_text = "Improperly formatted URL.";
		break;
	case 103:
		error_text = "Save In folder does not exist.";
		break;
	case 334:
		error_text = "Cannot create file. May be write protected or disk full.";
		break;

	case WSAEACCES: //(10013) 
		error_text = "Permission denied.";
		break;

	case WSAEADDRINUSE: //(10048) 
		error_text = "Address already in use.";
		break;

	case WSAEADDRNOTAVAIL: //(10049) 
		error_text = "Cannot assign requested address.";
		break;

	case WSAEAFNOSUPPORT: //(10047) 
		error_text = "Address family not supported by protocol family.";
		break;

	case WSAEALREADY: //(10037) 
		error_text = "Operation already in progress.";
		break;

	case WSAECONNABORTED: //(10053) 
		error_text = "Software caused connection abort."; 
		break;

	case WSAECONNREFUSED: //(10061) 
		error_text = "Connection refused.";
		break;

	case WSAECONNRESET: //(10054) 
		error_text = "Connection reset by peer."; 
		break;

	case WSAEDESTADDRREQ: //(10039) 
		error_text = "Destination address required.";
		break;

	case WSAEFAULT: //(10014) 
		error_text = "Bad address.";
		break;

	case WSAEHOSTDOWN: //(10064) 
		error_text = "Host is down.";
		break;

	case WSAEHOSTUNREACH: //(10065) 
		error_text = "No route to host.";
		break;

	case WSAEINPROGRESS: //(10036) 
		error_text = "Operation now in progress.";
		break;

	case WSAEINTR: //(10004) 
		error_text = "Interrupted function call.";
		break;

	case WSAEINVAL: //(10022) 
		error_text = "Invalid argument.";
		break;

	case WSAEISCONN: //(10056) 
		error_text = "Socket is already connected.";
		break;

	case WSAEMFILE: //(10024) 
		error_text = "Too many open files.";
		break;

	case WSAEMSGSIZE: //(10040) 
		error_text = "Message too long.";
		break;

	case WSAENETDOWN: //(10050) 
		error_text = "Network is down.";
		break;

	case WSAENETRESET: //(10052) 
		error_text = "Network dropped connection on reset.";
		break;

	case WSAENETUNREACH: //(10051) 
		error_text = "Network is unreachable.";
		break;

	case WSAENOBUFS: //(10055) 
		error_text = "No buffer space available.";
		break;

	case WSAENOPROTOOPT: //(10042) 
		error_text = "Bad protocol option.";
		break;

	case WSAENOTCONN: //(10057) 
		error_text = "Socket is not connected.";
		break;

	case WSAENOTSOCK: //(10038) 
		error_text = "Socket operation on non-socket."; 
		break;

	case WSAEOPNOTSUPP: //(10045) 
		error_text = "Operation not supported.";
		break;

	case WSAEPFNOSUPPORT: //(10046) 
		error_text = "Protocol family not supported.";
		break;

	case WSAEPROCLIM: //(10067) 
		error_text = "Too many processes.";
		break;

	case WSAEPROTONOSUPPORT: //(10043) 
		error_text = "Protocol not supported.";
		break;

	case WSAEPROTOTYPE: //(10041) 
		error_text = "Protocol wrong type for socket.";
		break;

	case WSAESHUTDOWN: //(10058) 
		error_text = "Cannot send after socket shutdown.";
		break;

	case WSAESOCKTNOSUPPORT: //(10044) 
		error_text = "Socket type not supported.";
		break;

	case WSAETIMEDOUT: //(10060) 
		error_text = "Connection timed out.";
		break;

	case WSAEWOULDBLOCK: //(10035) 
		error_text = "Resource temporarily unavailable.";
		break;

	case WSAHOST_NOT_FOUND: //(11001) 
		error_text = "Host not found.";
		break;

	case WSANOTINITIALISED: //(10093) 
		error_text = "Successful WSAStartup not yet performed.";
		break;

	case WSANO_DATA: //(11004) 
		error_text = "Valid name, no data record of requested type.";
		break;

	case WSANO_RECOVERY: //(11003) 
		error_text = "This is a non-recoverable error.";
		break;

	case WSASYSNOTREADY: //(10091) 
		error_text = "Network subsystem is unavailable.";
		break;

	case WSATRY_AGAIN: //(11002) 
		error_text = "Non-authoritative host not found.";
		break;

	case WSAVERNOTSUPPORTED: //(10092) 
		error_text = "WINSOCK.DLL version out of range.";
		break;

	case WSAEDISCON: //(10094) 
		error_text = "Graceful shutdown in progress.";
		break;

	default:
		error_text = "Unknown Error Number" + error_code;
		break;
	}

	return error_text;
}

int get_site_contents(map<string, string> *site, string url, int max_level, string save_in, string only_of_mime, string proxy_server, int proxy_port)
{
	SOCKET my_socket;
	string host;
	string start_point;
	map<string, string>::iterator p;
	int error_code;

	error_code = 0;
	my_socket = 0;
	site->clear();

	if(!break_url(url, &host, &start_point))
		error_code = 102;
	
	if(!error_code)
	{
		if(proxy_server == "" || proxy_port == 0)
			error_code = socket_connect(&my_socket, host, 80, 1);
		else
			error_code = socket_connect(&my_socket, proxy_server, proxy_port, 1);
	}
		
	if(!error_code)
	{
		site->insert(pair<string, string>(" ", host));
		crawl(&error_code, my_socket, host, start_point, 0, max_level, save_in, only_of_mime, site);
	}

	if(only_of_mime != "")
	{
and_again:
		p = site->begin();
		p++;
		while(p != site->end())	
		{
			if(!is_of_same_mime(only_of_mime, p->second))
			{
				site->erase(p->first);
				goto and_again;
			}

			p++;
		}
	}
	socket_disconnect(my_socket, 1);

	return error_code;
}

int get_file_contents(string *data, string url, string save_as, string proxy_server, int proxy_port)
{
	SOCKET my_socket;
	string host;
	string file;
	int error_code;
	int i;
	string type;
	string size;
	bool is_html;
	bool has_moved;

	error_code = 0;
	my_socket = 0;
	*data = "";

	if(!break_url(url, &host, &file))
		error_code = 102;

	if(!error_code)
	{
		if(proxy_server == "" || proxy_port == 0)
			error_code = socket_connect(&my_socket, host, 80, 1);
		else
			error_code = socket_connect(&my_socket, proxy_server, proxy_port, 1);
	}
		
	if(!error_code)
	{

		i = 0;
		has_moved = true;
		while(has_moved && i < 5)
		{
			error_code = get_file_header(my_socket, host, url, data);
			if(error_code)
				break;
			
			break_http_header(*data, &type, &size, &is_html, &has_moved);

			if(has_moved)
				break_url(type, &type, &url);

			i++;
		}

		if(!error_code)
		{
			if(save_as == "")
				error_code = get_file(my_socket, host, url, data, save_as, false);	
			else
				error_code = get_file(my_socket, host, url, data, save_as, true);	
		}
	}

	socket_disconnect(my_socket, 1);

	return error_code;
}
