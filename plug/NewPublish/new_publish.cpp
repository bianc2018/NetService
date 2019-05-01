#include "new_publish.h"
#include "new_publish_impl.h"

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
