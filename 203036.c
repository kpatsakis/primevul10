void agp_enable(struct agp_bridge_data *bridge, u32 mode)
{
	if (!bridge)
		return;
	bridge->driver->agp_enable(bridge, mode);
}