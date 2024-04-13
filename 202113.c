static void cbq_addprio(struct cbq_sched_data *q, struct cbq_class *cl)
{
	q->nclasses[cl->priority]++;
	q->quanta[cl->priority] += cl->weight;
	cbq_normalize_quanta(q, cl->priority);
}