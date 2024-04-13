static bool control_type_valid(void *control_type)
{
	struct powercap_control_type *pos = NULL;
	bool found = false;

	mutex_lock(&powercap_cntrl_list_lock);

	list_for_each_entry(pos, &powercap_cntrl_list, node) {
		if (pos == control_type) {
			found = true;
			break;
		}
	}
	mutex_unlock(&powercap_cntrl_list_lock);

	return found;
}