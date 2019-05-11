#ifndef NEW_PUBLISH_SERVICE_H
#define NEW_PUBLISH_SERVICE_H
//typedef std::function< void(HTTP_HANDLE handle, int err_code)> SERVER_HANDLER;
//#include "http_service.h"

#define WEB_PLUG_SERVICE_API  extern "C" __declspec( dllexport )

WEB_PLUG_SERVICE_API  void init();

WEB_PLUG_SERVICE_API  int np_get_news(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_get_now_user(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_get_sessions(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_get_users(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_get_new(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_login(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_register(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_logout(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_create_new(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_save_new(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_upload_new_image(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_new_like(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_add_new_comment(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_remove_new(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_remove_user(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_delete_user(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_delete_new(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_reset_password(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_remove_user(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_reset_user_permission(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_reset_new_status(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_delete_sessions(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_get_data(unsigned long long handle);
WEB_PLUG_SERVICE_API  int np_get_datas(unsigned long long handle);

WEB_PLUG_SERVICE_API  int np_before(unsigned long long handle);
WEB_PLUG_SERVICE_API  int np_after(unsigned long long handle);
#endif