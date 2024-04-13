copy_attr_check (const char *name, struct error_context *ctx)
{
	int action = attr_copy_action (name, ctx);
	return action == 0 || action == ATTR_ACTION_PERMISSIONS;
}