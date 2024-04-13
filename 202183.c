static int cbq_set_fopt(struct cbq_class *cl, struct tc_cbq_fopt *fopt)
{
	cbq_change_defmap(cl, fopt->split, fopt->defmap, fopt->defchange);
	return 0;
}