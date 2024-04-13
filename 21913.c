void tcf_qevent_destroy(struct tcf_qevent *qe, struct Qdisc *sch)
{
	if (qe->info.block_index)
		tcf_block_put_ext(qe->block, sch, &qe->info);
}