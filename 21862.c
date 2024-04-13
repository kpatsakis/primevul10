static bool tcf_proto_check_delete(struct tcf_proto *tp)
{
	if (tp->ops->delete_empty)
		return tp->ops->delete_empty(tp);

	tp->deleting = true;
	return tp->deleting;
}