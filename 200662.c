static inline int nlmsg_msg_size(int payload)
{
	return NLMSG_HDRLEN + payload;
}