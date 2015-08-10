/**
* This file is: 
*
* Copyright (C) 1999, 2002 Stephen Makonin.
* All Rights Reserved.
*
*/

#ifndef _INC_MCCHTTP
#define _INC_MCCHTTP

///#ifdef  __cplusplus
///extern "C" {
///#endif

const char *_stdcall open_url(
	char *error,			//must be a preallocated string about 256 characters
	char *url,				//must be a valid url i.e. http://www.company.com
	char *save_as,			//were to save the file, set to null if not needed
	char *proxy_address,	//the address of the proxy server, set to null if not needed
	long proxy_port);		//the port used by the proxy server, set to null if not needed
//RETURNS: 
//	the data of the requested file, 
//	NULL if there is an error, or
//	the value in save_as if save_as in not NULL

const char *_stdcall open_website(
	char *error,			//must be a preallocated string about 256 characters 
	char *url,				//must be a valid url i.e. http://www.company.com 
	char *save_in, 			//were to save the files, set to null if not needed
	long level,				//level of recursion, -1 for all
	char *only_of_type,		//limit by mime i.e. "image", set to null if not needed
	char *proxy_address, 	//the address of the proxy server, set to null if not needed
	long proxy_port);		//the port used by the proxy server, set to null if not needed
//RETURNS:
//	a delimited string filename\tmime (??? bytes)\r\n

///#ifdef  __cplusplus
///}
///#endif

#endif  /* _INC_MCCHTTP */