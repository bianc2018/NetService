#ifndef NEW_PUBLISH_SERVICE_H
#define NEW_PUBLISH_SERVICE_H
//typedef std::function< void(HTTP_HANDLE handle, int err_code)> SERVER_HANDLER;
//#include "http_service.h"

#define WEB_PLUG_SERVICE_API  extern "C" __declspec( dllexport )

WEB_PLUG_SERVICE_API  int np_get_news(unsigned long long handle);
WEB_PLUG_SERVICE_API  int np_get_now_user(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_before(unsigned long long handle);
WEB_PLUG_SERVICE_API  int np_after(unsigned long long handle);
#endif