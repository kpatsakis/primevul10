static void cbq_change_defmap(struct cbq_class *cl, u32 splitid, u32 def, u32 mask)
{
	struct cbq_class *split = NULL;

	if (splitid == 0) {
		if ((split = cl->split) == NULL)
			return;
		splitid = split->classid;
	}

	if (split == NULL || split->classid != splitid) {
		for (split = cl->tparent; split; split = split->tparent)
			if (split->classid == splitid)
				break;
	}

	if (split == NULL)
		return;

	if (cl->split != split) {
		cl->defmap = 0;
		cbq_sync_defmap(cl);
		cl->split = split;
		cl->defmap = def&mask;
	} else
		cl->defmap = (cl->defmap&~mask)|(def&mask);

	cbq_sync_defmap(cl);
}