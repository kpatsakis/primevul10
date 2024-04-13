HTTP_Setup(struct http *hp, struct ws *ws, struct vsl_log *vsl,
    enum VSL_tag_e  whence)
{
	http_Teardown(hp);
	hp->nhd = HTTP_HDR_FIRST;
	hp->logtag = whence;
	hp->ws = ws;
	hp->vsl = vsl;
}