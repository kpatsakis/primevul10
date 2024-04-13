static BOOL update_send_surface_bits(rdpContext* context,
                                     const SURFACE_BITS_COMMAND* surfaceBitsCommand)
{
	wStream* s;
	rdpRdp* rdp = context->rdp;
	BOOL ret = FALSE;
	update_force_flush(context);
	s = fastpath_update_pdu_init(rdp->fastpath);

	if (!s)
		return FALSE;

	if (!update_write_surfcmd_surface_bits(s, surfaceBitsCommand))
		goto out_fail;

	if (!fastpath_send_update_pdu(rdp->fastpath, FASTPATH_UPDATETYPE_SURFCMDS, s,
	                              surfaceBitsCommand->skipCompression))
		goto out_fail;

	update_force_flush(context);
	ret = TRUE;
out_fail:
	Stream_Release(s);
	return ret;
}