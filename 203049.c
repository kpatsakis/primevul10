int agp_copy_info(struct agp_bridge_data *bridge, struct agp_kern_info *info)
{
	memset(info, 0, sizeof(struct agp_kern_info));
	if (!bridge) {
		info->chipset = NOT_SUPPORTED;
		return -EIO;
	}

	info->version.major = bridge->version->major;
	info->version.minor = bridge->version->minor;
	info->chipset = SUPPORTED;
	info->device = bridge->dev;
	if (bridge->mode & AGPSTAT_MODE_3_0)
		info->mode = bridge->mode & ~AGP3_RESERVED_MASK;
	else
		info->mode = bridge->mode & ~AGP2_RESERVED_MASK;
	info->aper_base = bridge->gart_bus_addr;
	info->aper_size = agp_return_size();
	info->max_memory = bridge->max_memory_agp;
	info->current_memory = atomic_read(&bridge->current_memory_agp);
	info->cant_use_aperture = bridge->driver->cant_use_aperture;
	info->vm_ops = bridge->vm_ops;
	info->page_mask = ~0UL;
	return 0;
}