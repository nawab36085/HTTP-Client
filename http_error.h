/* 
	Copyright (C) 2016-2017  Nawab
	HTTP Client header.
	This file defines several errors which can be thrown by the HTTP Client implementation. 
	Users should include this file to identify the error in their program. 
	The list is small but can be extended by adding standard errors defined by HTTP.
	Its a free software and can be used and redistributed without any restriction.
*/

//Error to indicate the memory allocation failure in http client module.
#define HTTP_ERROR_MEM_ALLOC_FAILED 1
//Error to indicate if the module encountered an unexpected null pointer during execution.
#define HTTP_ERROR_NULL_POINTER 2
//Error to indicate if the HTTP request type provided is invalid.
#define HTTP_ERROR_INVALID_REQUEST_TYPE 3
//Error to indicate if the socket creation failed.
#define HTTP_ERROR_SOCKET_CREATION_FAILED 4
//Error to indicate if connection with server could not be established.
#define HTTP_ERROR_SERVER_CONN_FAILED 5
//Error to indicate if the write operation on the socket failed.
#define HTTP_ERROR_SOCKET_WRITE_FAILED 6
//Error to indicate if the receive operation on the socket failed.
#define HTTP_ERROR_REC_FAILED 7
