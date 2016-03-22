/* 
	Copyright (C) 2016-2017  Nawab
	Test file to demonstrate the use of HTTP Client provided along with this file.
	Its a free software and can be used and redistributed without any restriction.

*/


#include <stdio.h>
#include <string.h>
#include "http_client.h"
#include "http_error.h"

	//request->path = "index.php/_rest/api/addmeasurement/imei/356619053905912/network_mcc/se/network_mnc/24005/sim_mcc/se/sim_mnc/24007/phone_type/PHONE_TYPE_NONE/is_screen_on/9999/source_type/1/lat/58.3980175/lng/15.5570489/time/1.402564889263E12/speed/0.0/accuracy/30/start_new_trip/1/gsm_cellid/4909685/psc/79/rssi/18/lac/74/compass/-161.57/accelerometer/-0.56-4.77-8.52/magnetometer/11.94--50.88--16.74/light/168.84210205078125/";



void test_http_get(http_request* request)
{
	request->req_type = GET;
	request->hostname = "httpbin.org";//Setting the host name //"cmt.ericsson.net";//"www.spotthelost.com";
	request->ip = "54.175.219.8";//Setting the ip address of the server//"184.168.33.85";//"81.170.208.100";//"184.168.33.85";
	request->port_number = 80;//Setting the port number at which server is listening//6063;//80;
	request->path = "/get";//Relative url path for the post request
	request->data = NULL;

}

void test_http_post(http_request* request)
{
	request->req_type = POST;
	request->hostname = "httpbin.org";//Setting the host name //"cmt.ericsson.net";//"www.spotthelost.com";
	request->ip = "54.175.219.8";//Setting the ip address of the server//"184.168.33.85";//"81.170.208.100";//"184.168.33.85";
	request->port_number = 80;//Setting the port number at which server is listening//6063;//80;
	request->path = "/post";//Relative url path for the post request
	request->data = "phoneNumber=12345678&password=abcdefgh";//Data in the form-urlencoded format to be sent. 
}
int main(int argc, char** argv)
{
	int req = 0;
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
		
	//Initializing the http.
	http_request* request = http_init();
	if(req == GET)
	{
		test_http_get(request);
	}
	else if(req == POST)
	{
		test_http_post(request);
	}
	http_response* response = http_req(request, 0);
	print_http_response(response);
	
	//Removing
	http_deinit(request, response);
	return SUCCESS;
}
