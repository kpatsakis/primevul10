static bool tcf_proto_exists_destroying(struct tcf_chain *chain,
					struct tcf_proto *tp)
{
	u32 hash = destroy_obj_hashfn(tp);
	struct tcf_proto *iter;
	bool found = false;

	rcu_read_lock();
	hash_for_each_possible_rcu(chain->block->proto_destroy_ht, iter,
				   destroy_ht_node, hash) {
		if (tcf_proto_cmp(tp, iter)) {
			found = true;
			break;
		}
	}
	rcu_read_unlock();

	return found;
}