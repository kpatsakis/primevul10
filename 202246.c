int register_tcf_proto_ops(struct tcf_proto_ops *ops)
{
	struct tcf_proto_ops *t, **tp;
	int rc = -EEXIST;

	write_lock(&cls_mod_lock);
	for (tp = &tcf_proto_base; (t = *tp) != NULL; tp = &t->next)
		if (!strcmp(ops->kind, t->kind))
			goto out;

	ops->next = NULL;
	*tp = ops;
	rc = 0;
out:
	write_unlock(&cls_mod_lock);
	return rc;
}