cbq_class_lookup(struct cbq_sched_data *q, u32 classid)
{
	struct cbq_class *cl;

	for (cl = q->classes[cbq_hash(classid)]; cl; cl = cl->next)
		if (cl->classid == classid)
			return cl;
	return NULL;
}