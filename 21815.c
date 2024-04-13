static bool tcf_proto_check_kind(struct nlattr *kind, char *name)
{
	if (kind)
		return nla_strscpy(name, kind, IFNAMSIZ) < 0;
	memset(name, 0, IFNAMSIZ);
	return false;
}