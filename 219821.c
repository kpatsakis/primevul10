static int __init powercap_init(void)
{
	int result;

	result = seed_constraint_attributes();
	if (result)
		return result;

	return class_register(&powercap_class);
}