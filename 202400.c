static struct tc_action *create_a(int i)
{
	struct tc_action *act;

	act = kmalloc(sizeof(*act), GFP_KERNEL);
	if (act == NULL) {
		printk("create_a: failed to alloc!\n");
		return NULL;
	}
	memset(act, 0, sizeof(*act));
	act->order = i;
	return act;
}