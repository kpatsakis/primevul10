static inline int nlmsg_ok(const struct nlmsghdr *nlh, int remaining)
{
	return (remaining >= sizeof(struct nlmsghdr) &&
		nlh->nlmsg_len >= sizeof(struct nlmsghdr) &&
		nlh->nlmsg_len <= remaining);
}