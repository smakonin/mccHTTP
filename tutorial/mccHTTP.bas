Attribute VB_Name = "mccHTTP"
Option Explicit

Declare Function open_url Lib "mcchttp.dll" (ByVal error As String, ByVal url As String, ByVal save_as As String, ByVal proxy_address As String, ByVal proxy_port As Long) As String

'PARAMS:
'	error			-	must be a preallocated string about 256 characters
'	url				-	must be a valid url i.e. http://www.company.com
'	save_as			-	were to save the file, set to null if not needed
'	proxy_address	-	the address of the proxy server, set to null if not needed
'	proxy_port		-	the port used by the proxy server, set to null if not needed
'
'RETURNS: 
'	the data of the requested file, 
'	"" if there is an error, or
'	the value in save_as if save_as in not ""


Declare Function open_website Lib "mcchttp.dll" (ByVal error As String, ByVal url As String, ByVal save_in As String, ByVal level As Long, ByVal only_of_type As String, ByVal proxy_address As String, ByVal proxy_port As Long) As String

'PARAMS:
'	error			-	must be a preallocated string about 256 characters
'	url				-	must be a valid url i.e. http://www.company.com
'	save_in			-	were to save the files, set to null if not needed
'	level			-	level of recursion, -1 for all
'	only_of_type	-	limit by mime i.e. "image", set to null if not needed
'	proxy_address	-	the address of the proxy server, set to null if not needed
'	proxy_port		-	the port used by the proxy server, set to null if not needed
'
'RETURNS:
'	a delimited string filename<vbTab>mime (??? bytes)<vbCrLf>

