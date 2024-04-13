nsdate_to_strftime (EmpathyAdiumData *data, const gchar *nsdate)
{
	/* Convert from NSDateFormatter (http://www.stepcase.com/blog/2008/12/02/format-string-for-the-iphone-nsdateformatter/)
	 * to strftime supported by g_date_time_format.
	 * FIXME: table is incomplete, doc of g_date_time_format has a table of
	 *        supported tags.
	 * FIXME: g_date_time_format in GLib 2.28 does 0 padding by default, but
	 *        in 2.29.x we have to explictely request padding with %0x */
	static const gchar *convert_table[] = {
		"a", "%p", // AM/PM
		"A", NULL, // 0~86399999 (Millisecond of Day)

		"cccc", "%A", // Sunday/Monday/Tuesday/Wednesday/Thursday/Friday/Saturday
		"ccc", "%a", // Sun/Mon/Tue/Wed/Thu/Fri/Sat
		"cc", "%u", // 1~7 (Day of Week)
		"c", "%u", // 1~7 (Day of Week)

		"dd", "%d", // 1~31 (0 padded Day of Month)
		"d", "%d", // 1~31 (0 padded Day of Month)
		"D", "%j", // 1~366 (0 padded Day of Year)

		"e", "%u", // 1~7 (0 padded Day of Week)
		"EEEE", "%A", // Sunday/Monday/Tuesday/Wednesday/Thursday/Friday/Saturday
		"EEE", "%a", // Sun/Mon/Tue/Wed/Thu/Fri/Sat
		"EE", "%a", // Sun/Mon/Tue/Wed/Thu/Fri/Sat
		"E", "%a", // Sun/Mon/Tue/Wed/Thu/Fri/Sat

		"F", NULL, // 1~5 (0 padded Week of Month, first day of week = Monday)

		"g", NULL, // Julian Day Number (number of days since 4713 BC January 1)
		"GGGG", NULL, // Before Christ/Anno Domini
		"GGG", NULL, // BC/AD (Era Designator Abbreviated)
		"GG", NULL, // BC/AD (Era Designator Abbreviated)
		"G", NULL, // BC/AD (Era Designator Abbreviated)

		"h", "%I", // 1~12 (0 padded Hour (12hr))
		"H", "%H", // 0~23 (0 padded Hour (24hr))

		"k", NULL, // 1~24 (0 padded Hour (24hr)
		"K", NULL, // 0~11 (0 padded Hour (12hr))

		"LLLL", "%B", // January/February/March/April/May/June/July/August/September/October/November/December
		"LLL", "%b", // Jan/Feb/Mar/Apr/May/Jun/Jul/Aug/Sep/Oct/Nov/Dec
		"LL", "%m", // 1~12 (0 padded Month)
		"L", "%m", // 1~12 (0 padded Month)

		"m", "%M", // 0~59 (0 padded Minute)
		"MMMM", "%B", // January/February/March/April/May/June/July/August/September/October/November/December
		"MMM", "%b", // Jan/Feb/Mar/Apr/May/Jun/Jul/Aug/Sep/Oct/Nov/Dec
		"MM", "%m", // 1~12 (0 padded Month)
		"M", "%m", // 1~12 (0 padded Month)

		"qqqq", NULL, // 1st quarter/2nd quarter/3rd quarter/4th quarter
		"qqq", NULL, // Q1/Q2/Q3/Q4
		"qq", NULL, // 1~4 (0 padded Quarter)
		"q", NULL, // 1~4 (0 padded Quarter)
		"QQQQ", NULL, // 1st quarter/2nd quarter/3rd quarter/4th quarter
		"QQQ", NULL, // Q1/Q2/Q3/Q4
		"QQ", NULL, // 1~4 (0 padded Quarter)
		"Q", NULL, // 1~4 (0 padded Quarter)

		"s", "%S", // 0~59 (0 padded Second)
		"S", NULL, // (rounded Sub-Second)

		"u", "%Y", // (0 padded Year)

		"vvvv", "%Z", // (General GMT Timezone Name)
		"vvv", "%Z", // (General GMT Timezone Abbreviation)
		"vv", "%Z", // (General GMT Timezone Abbreviation)
		"v", "%Z", // (General GMT Timezone Abbreviation)

		"w", "%W", // 1~53 (0 padded Week of Year, 1st day of week = Sunday, NB, 1st week of year starts from the last Sunday of last year)
		"W", NULL, // 1~5 (0 padded Week of Month, 1st day of week = Sunday)

		"yyyy", "%Y", // (Full Year)
		"yyy", "%y", // (2 Digits Year)
		"yy", "%y", // (2 Digits Year)
		"y", "%Y", // (Full Year)
		"YYYY", NULL, // (Full Year, starting from the Sunday of the 1st week of year)
		"YYY", NULL, // (2 Digits Year, starting from the Sunday of the 1st week of year)
		"YY", NULL, // (2 Digits Year, starting from the Sunday of the 1st week of year)
		"Y", NULL, // (Full Year, starting from the Sunday of the 1st week of year)

		"zzzz", NULL, // (Specific GMT Timezone Name)
		"zzz", NULL, // (Specific GMT Timezone Abbreviation)
		"zz", NULL, // (Specific GMT Timezone Abbreviation)
		"z", NULL, // (Specific GMT Timezone Abbreviation)
		"Z", "%z", // +0000 (RFC 822 Timezone)
	};
	const gchar *str;
	GString *string;
	guint i, j;

	if (nsdate == NULL) {
		return NULL;
	}

	str = g_hash_table_lookup (data->date_format_cache, nsdate);
	if (str != NULL) {
		return str;
	}

	/* Copy nsdate into string, replacing occurences of NSDateFormatter tags
	 * by corresponding strftime tag. */
	string = g_string_sized_new (strlen (nsdate));
	for (i = 0; nsdate[i] != '\0'; i++) {
		gboolean found = FALSE;

		/* even indexes are NSDateFormatter tag, odd indexes are the
		 * corresponding strftime tag */
		for (j = 0; j < G_N_ELEMENTS (convert_table); j += 2) {
			if (g_str_has_prefix (nsdate + i, convert_table[j])) {
				found = TRUE;
				break;
			}
		}
		if (found) {
			/* If we don't have a replacement, just ignore that tag */
			if (convert_table[j + 1] != NULL) {
				g_string_append (string, convert_table[j + 1]);
			}
			i += strlen (convert_table[j]) - 1;
		} else {
			g_string_append_c (string, nsdate[i]);
		}
	}

	DEBUG ("Date format converted '%s' → '%s'", nsdate, string->str);

	/* The cache takes ownership of string->str */
	g_hash_table_insert (data->date_format_cache, g_strdup (nsdate), string->str);
	return g_string_free (string, FALSE);
}