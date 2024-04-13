http_fail(const struct http *hp)
{
	char id[WS_ID_SIZE];

	VSC_C_main->losthdr++;
	WS_Id(hp->ws, id);
	VSLb(hp->vsl, SLT_Error, "out of workspace (%s)", id);
	WS_MarkOverflow(hp->ws);
}