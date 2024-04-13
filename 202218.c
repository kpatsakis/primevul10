int unregister_tcf_proto_ops(struct tcf_proto_ops *ops)
{
	struct tcf_proto_ops *t, **tp;
	int rc = -ENOENT;

	write_lock(&cls_mod_lock);
	for (tp = &tcf_proto_base; (t=*tp) != NULL; tp = &t->next)
		if (t == ops)
			break;

	if (!t)
		goto out;
	*tp = t->next;
	rc = 0;
out:
	write_unlock(&cls_mod_lock);
	return rc;
}