/***************************************INCLUDES**********************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "http_client.h"
#include "http_error.h"

/*******************************************FUNCTION PROTOTYPES*******************************************/
/********************************************************************************************
    Function Name:                      process_http_request
    Description:                        processes the http request by opening a socket and
                                        connecting to the server
    Parameters:                         http_request* - pointer to a structure
                                            containing http request info
                                        char* - pointer to the prepared request buffer
                                        http_response* - pointer to a structure
                                            for http response values
                                        int - to indicate if only raw response is required.
    Return:                             int - status of the processing of request
********************************************************************************************/
static int process_http_request(http_request* request, char* http_request_buffer, http_response* response, int raw);
/********************************************************************************************
    Function Name:                      process_http_response
    Description:                        parses the http response received from the server
    Parameters:                         char* - pointer to the prepared request buffer
                                        http_response* - pointer to a structure
                                            for http response values
    Return:                             int - status of the parsing of the response
********************************************************************************************/

static int process_http_response(char* http_response_buffer, http_response* response);

/***************************************FUNCTION IMPLEMENTATIONS**********************************************/
/*******************************************************************************************
    Function Name:                      http_init
    Description:                        initializes required data structures
    Parameters:                         void
    Return:                             http_response* - pointer to a structure
                                            containing http response
********************************************************************************************/
http_request* http_init(void)
{
	http_request* request = (http_request*)malloc(sizeof(http_request));
	if(request == NULL)
	{
		printf("Memory allocation for http_request failed.\n");
		return NULL;
	}
	return request;
}

/********************************************************************************************
    Function Name:                      http_deinit
    Description:                        Frees allocated memory for data structures
    Parameters:                         http_request* - pointer to a structure
                                            containing http request info
                                        http_response* - pointer to a structure
                                            containing http response
    Return:                             void
********************************************************************************************/
void http_deinit(http_request* request, http_response* response)
{
	if(request)
		free(request);

	if(response == NULL)
		printf("delete_http_response: response is NULL.\n");

	if(response->status_text)
		free(response->status_text);
	if(response->body)
		free(response->body);
	if(response->server_name)
		free(response->server_name);
	if(response->content_type)
		free(response->content_type);
	if(response->raw_response)
		free(response->raw_response);

	free(response);
}

/********************************************************************************************
    Function Name:                      http_req
    Description:                        facilitates the http request by indentifying
                                        the http request type
    Parameters:                         http_request* - pointer to a structure
                                            containing http request info
                                        int - to indicate if only raw response is required.
    Return:                             http_response* - pointer to a structure
                                            containing http response
********************************************************************************************/
http_response* http_req(http_request* request, int raw)
{
	if(request == NULL)
	{
		printf("http_request: request is NULL.\n");
		return NULL;
	}
	http_response* response = (http_response*)malloc(sizeof(http_response));
	if(response == NULL)
	{
		printf("http_request: memory allocation for http response failed.\n");
		return NULL;
	}
	//int status;
	char http_request_buffer[REQUEST_BUFF_SIZE];
	memset(http_request_buffer, 0, REQUEST_BUFF_SIZE);
	switch(request->req_type)
	{
		case GET:
			sprintf(http_request_buffer, "GET /%s HTTP/1.1\r\nHost:%s\r\n\r\n", request->path, request->hostname);
			break;
		case POST:
			//sprintf(http_request_buffer, "POST /%s HTTP/1.1\r\nHost: %s\r\n\Content-Length: %zu\r\nContent-Type: application/x-www-form-urlencoded\r\n%s\r\n\r\n", request->path, request->hostname, strlen(request->data), request->data);
			sprintf(http_request_buffer, "POST /%s HTTP/1.1\r\nHost:%s\r\nContent-Length: %d\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n%s", request->path, request->hostname, (int)strlen(request->data), request->data);
			break;
		case HEAD:
			//TODO
			break;
		case PUT:
			//TODO
			break;
		case DELETE:
			//TODO
			break;
		case CONNECT:
			//TODO
			break;
		case OPTIONS:
			//TODO
			break;
		case TRACE:
			//TODO
			break;
		default:
			printf("http_request: It is not a valid HTTP request type.\n");
	}
	process_http_request(request, http_request_buffer, response, raw);
	return response;
}


/********************************************************************************************
    Function Name:                      process_http_request
    Description:                        processes the http request by opening a socket and
                                        connecting to the server
    Parameters:                         http_request* - pointer to a structure
                                            containing http request info
                                        char* - pointer to the prepared request buffer
                                        http_response* - pointer to a structure
                                            for http response values
                                        int - to indicate if only raw response is required.
    Return:                             int - status of the processing of request
********************************************************************************************/
static int process_http_request(http_request* request, char* http_request_buffer, http_response* response, int raw)
{
	int socketfd ;
	struct sockaddr_in socket_details;
	char temp_buffer[TEMP_BUFF_SIZE];
	char* http_response_buffer;
	ssize_t bytes_sent = 0;
	ssize_t total_bytes_sent = 0;
	ssize_t bytes_received = 0;
	ssize_t total_bytes_received = 0;
	int status;
	//Allocating memory for the raw http response.
	http_response_buffer = (char*)malloc(RESPONSE_BUFF_SIZE);
	if (http_response_buffer == NULL )
	{
		printf ("process_http_request: memory allocation failed for response buffer.\n");
		return HTTP_ERROR_MEM_ALLOC_FAILED;
	}
	memset(http_response_buffer, 0, RESPONSE_BUFF_SIZE);

	//Creating the client socket.
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd == -1)
	{
		printf("process_http_request: socket creation failed.\n");
		return HTTP_ERROR_SOCKET_CREATION_FAILED;
	}

	//Setting the receive timeout option for the socket. Should be changed as per requirement.
	//If not set, the recv call on the socket will keep blocking for data.
    struct timeval timeout;      
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
	if(setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
        printf("process_http_request: socket option setting failed.\n");

	//Preparing connection information for connecting to server socket.
	//memset(&socket_details , 0 , sizeof(struct sockaddr_in));
	bzero(&socket_details,sizeof(socket_details));
	socket_details.sin_family = AF_INET ;
	//printf("ip is %s, port is %d\n", request->ip, request->port_number);
	socket_details.sin_addr.s_addr=inet_addr(request->ip);
	socket_details.sin_port = htons(request->port_number);

	//Connecting to the server socket using client socket and connection information.
	if(connect(socketfd, (struct sockaddr*)&socket_details, sizeof(struct sockaddr)) == -1)
	{
		printf("process_http_request: Could not connect to the server.\n");
		return HTTP_ERROR_SERVER_CONN_FAILED;
	}

	//Sending the data to the server.
	while(1)
	{
		bytes_sent = send(socketfd,http_request_buffer,strlen(http_request_buffer), 0);
		if(bytes_sent < 0)
		{

			printf("process_http_request: error writing on socket\n");
			close(socketfd);
			return HTTP_ERROR_SOCKET_WRITE_FAILED;
		}
		else
		{
			if(LOG_LEVEL >= DEBUG)
				printf("process_http_request: wrote %d bytes data on socket successfully.\n", (int)bytes_sent);
		}
		total_bytes_sent += (int)bytes_sent;
		printf("process_http_request: total bytes sent = %d\n", (int)total_bytes_sent);
		if ( total_bytes_sent == strlen(http_request_buffer)) 
			break;
	}

	//Receiving the data from the server.
	while(1)
	{
		if(LOG_LEVEL >= INFO)
			printf("process_http_request: receiving bytes from server\n");
		memset(temp_buffer, 0, TEMP_BUFF_SIZE);
		bytes_received = recv(socketfd, temp_buffer, TEMP_BUFF_SIZE, 0);
		if(bytes_received == -1)
		{
			if(total_bytes_received == 0)
			{
				printf ("process_http_request: Data could not be received from socket.\n");
				close(socketfd);
				return HTTP_ERROR_REC_FAILED;
			}
			else
			{
				if(LOG_LEVEL >= INFO)
					printf ("process_http_request: Timeout waiting for more data.\n");
				break;
			}
		}
		else if(bytes_received > 0)
		{
			sprintf(http_response_buffer, "%s%s", http_response_buffer, temp_buffer);
			total_bytes_received += bytes_received;
			//if(LOG_LEVEL >= DEBUG)
			printf("process_http_request: total bytes received = %d\n", (int)total_bytes_received);
		}
		if (bytes_received == 0) 
		{
			if(LOG_LEVEL >= INFO)
				printf("process_http_request: Data receiving finished.\n");
			close(socketfd);
			break;
		}	
	}
	if(LOG_LEVEL >= INFO)
		printf ("raw buffer=%s", http_response_buffer) ;
	response->raw_response = http_response_buffer;
	//Calling the function to process the http response received from the server.
	if(raw == 0)
	{
		status = process_http_response(http_response_buffer, response);
		if(status == SUCCESS)
			printf("process_http_request: HTTP response parsed successfully.\n");
		else
			printf("process_http_request: HTTP response parsing encountered problem. \n");
	}
	else
	{
		status = SUCCESS;
	}
	return status ;

}

/********************************************************************************************
    Function Name:                      process_http_response
    Description:                        parses the http response received from the server
    Parameters:                         char* - pointer to the prepared request buffer
                                        http_response* - pointer to a structure
                                            for http response values
    Return:                             int - status of the parsing of the response
********************************************************************************************/
static int process_http_response(char* http_response_buffer, http_response* response)
{
	if(response == NULL)
	{
		printf("process_http_response: response pointer is null.\n");
		return HTTP_ERROR_NULL_POINTER;
	}
	if(LOG_LEVEL >= DEBUG)
		printf("processing body.\n");
	int len;
	char* body = strstr(http_response_buffer, "\r\n\r\n");
	if(body)
	{
		body = body + 4;
		len = strlen(body) + 1;
		response->body = malloc(len);
		memset(response->body, 0, len);
		memcpy(response->body, body, len);
	}

	if(LOG_LEVEL >= DEBUG)
		printf("processing status code and text.\n");
	char* status_start = strstr(http_response_buffer, "HTTP/1.1 ");
	char* status_end;
	if(status_start)
	{
		status_end = strstr(http_response_buffer, "\r\n");
		if(status_end != NULL)
		{
			status_start = status_start + 9;
			len = status_end - status_start;
			char* status_code_end = strchr(status_start, ' ');
			if(status_code_end)
			{
				len = status_code_end - status_start;
				if(len > 0)
				{
					char code[len+1];
					memset(code, 0, len+1);
					memcpy(code, status_start, len);
					response->status_code = strtol(code, NULL, 10);

					len = status_end - (status_code_end + 1);
					response->status_text = malloc(len + 1);
					memset(response->status_text, 0, len + 1);
					memcpy(response->status_text, status_code_end + 1, len);
				}
			}
		}
	}

	if(LOG_LEVEL >= DEBUG)
		printf("processing server.\n");
	char* server_start = strstr(http_response_buffer, "Server: ");
	char* server_end;
	if(server_start)
	{
		server_start = server_start + 8;
		server_end = strstr(server_start, "\r\n");
		if(server_end)
		{
			len = server_end - server_start + 1;
			response->server_name = malloc(len);
			memset(response->server_name, 0, len);
			memcpy(response->server_name, server_start, len);
		}
	}

	if(LOG_LEVEL >= DEBUG)
		printf("processing conent length.\n");
	char* content_length_start = strstr(http_response_buffer, "Content-Length: ");
	char* content_length_end;
	if(content_length_start)
	{
		content_length_start = content_length_start + 16;
		content_length_end = strstr(content_length_start, "\r\n");
		if(content_length_end)
		{
			len = content_length_end - content_length_start;
			if(len > 0)
			{
				char content_length[len+1];
				memset(content_length, len+1, 0);
				memcpy(content_length, content_length_start, len);
				//*content_length_end = 0;
				response->content_length = strtol(content_length, NULL, 10);
			}
		}
	}

	if(LOG_LEVEL >= DEBUG)
		printf("processing content type.\n");
	//http_response_buffer = content_length_end + 2;
	char* content_type_start = strstr(http_response_buffer, "Content-Type: ");
	if(content_type_start)
	{
		content_type_start = content_type_start + 14;
		char* content_type_end = strstr(content_type_start, "\r\n");
		//*content_type_end = 0;
		if(content_type_end)
		{
			len = content_type_end - content_type_start;
			response->content_type = malloc(len + 1);
			memset(response->content_type, 0, len + 1);
			memcpy(response->content_type, content_type_start, len);
		}
	}
	return SUCCESS;
}

/********************************************************************************************
    Function Name:                      print_http_response
    Description:                        prints the parsed http response received from the server
    Parameters:                         http_response* - pointer to a structure
                                            containing http response
    Return:                             void
********************************************************************************************/
void print_http_response(http_response* response)
{
	if(response == NULL)
		printf("response pointer is null.\n");
	printf("raw_response=%s\n", response->raw_response);
	printf("status_code=%d\n", response->status_code);
	printf("status_text=%s\n", response->status_text);
	printf("server_name=%s\n", response->server_name);
	printf("Content-Length=%d\n", response->content_length);
	printf("Content-Type=%s\n", response->content_type);
	//printf("body=%s\n", response->body);
}
