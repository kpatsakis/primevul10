}

static char *format_date(u64 time, char *szTime)
{
	time_t now;
	if (!time) {
		strcpy(szTime, "UNKNOWN DATE");
	} else {
		time -= 2082844800;
		now = (u32) time;
		sprintf(szTime, "GMT %s", asctime(gf_gmtime(&now)) );
	}