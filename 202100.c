static void cbq_sync_defmap(struct cbq_class *cl)
{
	struct cbq_sched_data *q = qdisc_priv(cl->qdisc);
	struct cbq_class *split = cl->split;
	unsigned h;
	int i;

	if (split == NULL)
		return;

	for (i=0; i<=TC_PRIO_MAX; i++) {
		if (split->defaults[i] == cl && !(cl->defmap&(1<<i)))
			split->defaults[i] = NULL;
	}

	for (i=0; i<=TC_PRIO_MAX; i++) {
		int level = split->level;

		if (split->defaults[i])
			continue;

		for (h=0; h<16; h++) {
			struct cbq_class *c;

			for (c = q->classes[h]; c; c = c->next) {
				if (c->split == split && c->level < level &&
				    c->defmap&(1<<i)) {
					split->defaults[i] = c;
					level = c->level;
				}
			}
		}
	}
}