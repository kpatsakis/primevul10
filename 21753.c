http_SetStatus(struct http *to, uint16_t status, const char *reason)
{
	char buf[4];
	const char *sstr = NULL;

	CHECK_OBJ_NOTNULL(to, HTTP_MAGIC);
	/*
	 * We allow people to use top digits for internal VCL
	 * signalling, but strip them from the ASCII version.
	 */
	to->status = status;
	status %= 1000;
	assert(status >= 100);

	if (reason == NULL)
		reason = http_Status2Reason(status, &sstr);
	else
		(void)http_Status2Reason(status, &sstr);

	if (sstr) {
		http_SetH(to, HTTP_HDR_STATUS, sstr);
	} else {
		bprintf(buf, "%03d", status);
		http_PutField(to, HTTP_HDR_STATUS, buf);
	}
	http_SetH(to, HTTP_HDR_REASON, reason);
}