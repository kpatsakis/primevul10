do_readlink(
	char __user		*buffer,
	int			buflen,
	const char		*link)
{
        int len;

	len = PTR_ERR(link);
	if (IS_ERR(link))
		goto out;

	len = strlen(link);
	if (len > (unsigned) buflen)
		len = buflen;
	if (copy_to_user(buffer, link, len))
		len = -EFAULT;
 out:
	return len;
}