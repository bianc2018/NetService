#include "new_publish.h"
#include "new_publish_impl.h"

WEB_PLUG_SERVICE_API void init()
{
	NewPublishImpl &impl = NewPublishImpl::instance();
}

WEB_PLUG_SERVICE_API int np_get_news(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_get_news(handle);
}

WEB_PLUG_SERVICE_API int np_get_now_user(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_get_now_user(handle);
}

WEB_PLUG_SERVICE_API int np_get_sessions(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_get_sessions(handle);
}

WEB_PLUG_SERVICE_API int np_get_users(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_get_users(handle);
}

WEB_PLUG_SERVICE_API int np_get_new(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_get_new(handle);
}

WEB_PLUG_SERVICE_API int np_login(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_login(handle);
}

WEB_PLUG_SERVICE_API int np_register(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_register(handle);
}

WEB_PLUG_SERVICE_API int np_logout(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_logout(handle);
}

WEB_PLUG_SERVICE_API int np_create_new(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_create_new(handle);
}

WEB_PLUG_SERVICE_API int np_save_new(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_save_new(handle);
}

WEB_PLUG_SERVICE_API int np_upload_new_image(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_upload_new_image(handle);
}

WEB_PLUG_SERVICE_API int np_new_like(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_new_like(handle);
}

WEB_PLUG_SERVICE_API int np_add_new_comment(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_add_new_comment(handle);
}

WEB_PLUG_SERVICE_API int np_remove_new(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_remove_new(handle);
}

WEB_PLUG_SERVICE_API int np_remove_user(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_remove_user(handle);
}

WEB_PLUG_SERVICE_API int np_reset_user_permission(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_reset_user_permission(handle);
}

WEB_PLUG_SERVICE_API int np_reset_new_status(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_reset_new_status(handle);
}

WEB_PLUG_SERVICE_API int np_delete_sessions(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_detele_sessions(handle);
}

WEB_PLUG_SERVICE_API int np_delete_user(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_delete_user(handle);
}

WEB_PLUG_SERVICE_API int np_delete_new(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_delete_new(handle);
}

WEB_PLUG_SERVICE_API int np_reset_password(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.deal_reset_password(handle);
}

WEB_PLUG_SERVICE_API int np_before(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.before_filter(handle);
}

WEB_PLUG_SERVICE_API int np_after(unsigned long long handle)
{
	NewPublishImpl &impl = NewPublishImpl::instance();
	return impl.after_filter(handle);
}
