/* 
	Copyright (C) 2016-2017  Nawab
	Test file to demonstrate the use of HTTP Client provided along with this file.
	Its a free software and can be referred, used and redistributed without any restriction.

*/

#include <stdio.h>
#include <string.h>
#include "http_client.h"
#include "http_error.h"



void test_http_get(http_request* request)
{
	request->req_type = GET;
	request->hostname = "httpbin.org";//Setting the host name 
	request->ip = "54.175.219.8";//Setting the ip address of the server
	request->port_number = 80;//Setting the port number at which server is listening
	request->path = "/get";//Relative url path for the get request
	request->data = NULL;//No data should be passed for GET request and it will not be used while making the request.

}

void test_http_post(http_request* request)
{
	request->req_type = POST;
	request->hostname = "httpbin.org";//Setting the host name
	request->ip = "54.175.219.8";//Setting the ip address of the server
	request->port_number = 80;//Setting the port number at which server is listening
	request->path = "/post";//Relative url path for the post request
	request->data = "Name=Nawab&City=Berlin";//Data in the form-urlencoded format to be sent. 
}
int main(int argc, char** argv)
{
	int req = GET;//Assigining default value GET in case the request type is not provided in the argument
	if(argc < 2)
		printf("Insufficient number of arguments, so assuming GET as default request.\n");
	if(argv[1] != NULL)
	{
		if(strcasecmp(argv[1], "get") == 0)
		{
			req = GET;
		}
		else if(strcasecmp(argv[1], "post") == 0)
		{
			req = POST;
		}
	}
		
	//Allocating resources for the HTTP request operation.
	http_request* request = http_init();
	if(req == GET)
	{
		//Setting GET request parameters.
		test_http_get(request);
	}
	else if(req == POST)
	{
		//Setting POST request parameters.
		test_http_post(request);
	}
	//Making the HTTP request with request parameters filled in the request structure
	// and asking for decoded http response by passing second argument raw as 0.
	http_response* response = http_req(request, 0);
	//Printing the http response.
	print_http_response(response);
	
	//Freeing the resources.
	http_deinit(request, response);
	return SUCCESS;
}
