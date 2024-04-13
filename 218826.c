static BOOL update_send_pointer_system(rdpContext* context,
                                       const POINTER_SYSTEM_UPDATE* pointer_system)
{
	wStream* s;
	BYTE updateCode;
	rdpRdp* rdp = context->rdp;
	BOOL ret;
	s = fastpath_update_pdu_init(rdp->fastpath);

	if (!s)
		return FALSE;

	if (pointer_system->type == SYSPTR_NULL)
		updateCode = FASTPATH_UPDATETYPE_PTR_NULL;
	else
		updateCode = FASTPATH_UPDATETYPE_PTR_DEFAULT;

	ret = fastpath_send_update_pdu(rdp->fastpath, updateCode, s, FALSE);
	Stream_Release(s);
	return ret;
}