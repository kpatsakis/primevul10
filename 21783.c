http_Status2Reason(unsigned status, const char **sstr)
{
	struct http_msg *mp;

	status %= 1000;
	assert(status >= 100);
	for (mp = http_msg; mp->nbr != 0 && mp->nbr <= status; mp++)
		if (mp->nbr == status) {
			if (sstr)
				*sstr = mp->status;
			return (mp->txt);
		}
	return ("Unknown HTTP Status");
}