static bool tcf_proto_is_deleting(struct tcf_proto *tp)
{
	bool deleting;

	spin_lock(&tp->lock);
	deleting = tp->deleting;
	spin_unlock(&tp->lock);

	return deleting;
}