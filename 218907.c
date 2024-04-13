static BOOL update_send_desktop_resize(rdpContext* context)
{
	return rdp_server_reactivate(context->rdp);
}