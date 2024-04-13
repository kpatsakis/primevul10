static inline struct neigh_parms *lookup_neigh_params(struct neigh_table *tbl,
						      int ifindex)
{
	struct neigh_parms *p;
	
	for (p = &tbl->parms; p; p = p->next)
		if ((p->dev && p->dev->ifindex == ifindex) ||
		    (!p->dev && !ifindex))
			return p;

	return NULL;
}