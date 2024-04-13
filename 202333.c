void neigh_parms_destroy(struct neigh_parms *parms)
{
	kfree(parms);
}