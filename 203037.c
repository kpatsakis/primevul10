unsigned long agp_generic_mask_memory(struct agp_bridge_data *bridge,
				      dma_addr_t addr, int type)
{
	/* memory type is ignored in the generic routine */
	if (bridge->driver->masks)
		return addr | bridge->driver->masks[0].mask;
	else
		return addr;
}