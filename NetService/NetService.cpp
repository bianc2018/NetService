#include "../HttpService/src/http_service.h"
//»Øµ÷º¯Êý
#include <iostream>

void server_handler(HTTP_HANDLE handle, req_ptr requset, res_ptr response)
{
	std::cout << "handle=" << handle <<" "<< requset->uri<<std::endl;
}
int main()
{
	HTTP_SERVICE_start_server("0.0.0.0", 8000, server_handler, 16);
	HTTP_SERVICE_run();
}