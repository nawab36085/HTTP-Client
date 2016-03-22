
/**********************************************DEFINES**********************************************************/
//To signifies if the operation succeeded.
#define SUCCESS 0
//To signifies if the operation failed.
#define FAILURE 1
//Buffer size to store the HTTP request. Should be changed as per requirement.
#define REQUEST_BUFF_SIZE 2048
//Buffer size to store the HTTP response received from server. Should be changed as per requirement.
#define RESPONSE_BUFF_SIZE 10000
//Buffer size to store every chunk of data received from server.
#define TEMP_BUFF_SIZE 2048

#define NONE	0
#define ERROR 	1
#define WARN	2
#define DEBUG	3
#define INFO	4
#define VERBOSE 5
#define LOG_LEVEL NONE

/**********************************************ENUMS************************************************************/
//Enum to list all the HTTP request types.
typedef enum request_type
{
	GET,
	POST,
	HEAD,
	PUT,
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE
}request_type;

/*********************************************STRUCTURES*******************************************************/
//Structure to hold information regarding an HTTP request
typedef struct http_request
{
	//Type of the HTTP request - mandatory
	request_type req_type;
	//IP address of the server - mandatory
	char* ip;
	//Port number at which server is listening - mandatory
	int port_number;
	//url path without the hostname
	char* path;
	//data associated with the request.
	char* data;
	//Name of the host being requested - optional
	char* hostname;
	//Username as part of HTTP request for authentication - optional
	//char* username;//Not using now
	//Password as part of HTTP request for authentication - optional
	//char* password;//Not using now
}http_request;

//Structure to hold information parsed from the HTTP response received from server
typedef struct http_response
{
	//raw response from the server.
	char* raw_response;
	//The status code received for the HTTP request from the server
	int status_code;
	//The text of the status of the HTTP request 
	char* status_text;
	//The body of the HTTP response received from server
	char* body;
	//char* header;//Not using right now
	//Name of the server responding to the HTTP request
	char* server_name;
	//char* timestamp;//Not using right now
	//Length of the content received in the HTTP response
	int content_length;
	//Type of the content received in the HTTP response from server
	char* content_type;
}http_response;

/************************************************FUNCTION PROTOTYPES*****************************************/

/******************************************************************************
    Function Name:                      http_init
    Description:                        initializes required data structures
    Parameters:                         void
    Return:                             http_response* - pointer to a structure
                                            containing http response
*******************************************************************************/
http_request* http_init();
/********************************************************************************************
    Function Name:                      http_req
    Description:                        facilitates the http request by indentifying
                                        the http request type
    Parameters:                         http_request* - pointer to a structure
                                            containing http request info
    Return:                             http_response* - pointer to a structure
                                            containing http response
                                        int - to indicate if only raw response is required.
********************************************************************************************/
http_response* http_req(http_request* request, int raw);

/******************************************************************************
    Function Name:                      http_deinit
    Description:                        Frees allocated memory for data structures
    Parameters:                         http_request* - pointer to a structure
                                            containing http request info
                                        http_response* - pointer to a structure
                                            containing http response
    Return:                             void
*******************************************************************************/
void http_deinit(http_request* request, http_response* response);

/********************************************************************************************
    Function Name:                      print_http_response
    Description:                        prints the parsed http response received from the server
    Parameters:                         http_response* - pointer to a structure
                                            containing http response
    Return:                             void
********************************************************************************************/
void print_http_response(http_response* response);
