find_dump_kind(struct nlmsghdr *n)
{
	struct rtattr *tb1, *tb2[TCA_ACT_MAX+1];
	struct rtattr *tb[TCA_ACT_MAX_PRIO + 1];
	struct rtattr *rta[TCAA_MAX + 1];
	struct rtattr *kind;
	int min_len = NLMSG_LENGTH(sizeof(struct tcamsg));
	int attrlen = n->nlmsg_len - NLMSG_ALIGN(min_len);
	struct rtattr *attr = (void *) n + NLMSG_ALIGN(min_len);

	if (rtattr_parse(rta, TCAA_MAX, attr, attrlen) < 0)
		return NULL;
	tb1 = rta[TCA_ACT_TAB - 1];
	if (tb1 == NULL)
		return NULL;

	if (rtattr_parse(tb, TCA_ACT_MAX_PRIO, RTA_DATA(tb1),
	                 NLMSG_ALIGN(RTA_PAYLOAD(tb1))) < 0)
		return NULL;
	if (tb[0] == NULL)
		return NULL;

	if (rtattr_parse(tb2, TCA_ACT_MAX, RTA_DATA(tb[0]),
	                 RTA_PAYLOAD(tb[0])) < 0)
		return NULL;
	kind = tb2[TCA_ACT_KIND-1];

	return (char *) RTA_DATA(kind);
}