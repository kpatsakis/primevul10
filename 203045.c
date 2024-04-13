int agp_generic_type_to_mask_type(struct agp_bridge_data *bridge,
				  int type)
{
	if (type >= AGP_USER_TYPES)
		return 0;
	return type;
}