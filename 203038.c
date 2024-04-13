int agp_generic_create_gatt_table(struct agp_bridge_data *bridge)
{
	char *table;
	char *table_end;
	int size;
	int page_order;
	int num_entries;
	int i;
	void *temp;
	struct page *page;

	/* The generic routines can't handle 2 level gatt's */
	if (bridge->driver->size_type == LVL2_APER_SIZE)
		return -EINVAL;

	table = NULL;
	i = bridge->aperture_size_idx;
	temp = bridge->current_size;
	size = page_order = num_entries = 0;

	if (bridge->driver->size_type != FIXED_APER_SIZE) {
		do {
			switch (bridge->driver->size_type) {
			case U8_APER_SIZE:
				size = A_SIZE_8(temp)->size;
				page_order =
				    A_SIZE_8(temp)->page_order;
				num_entries =
				    A_SIZE_8(temp)->num_entries;
				break;
			case U16_APER_SIZE:
				size = A_SIZE_16(temp)->size;
				page_order = A_SIZE_16(temp)->page_order;
				num_entries = A_SIZE_16(temp)->num_entries;
				break;
			case U32_APER_SIZE:
				size = A_SIZE_32(temp)->size;
				page_order = A_SIZE_32(temp)->page_order;
				num_entries = A_SIZE_32(temp)->num_entries;
				break;
				/* This case will never really happen. */
			case FIXED_APER_SIZE:
			case LVL2_APER_SIZE:
			default:
				size = page_order = num_entries = 0;
				break;
			}

			table = alloc_gatt_pages(page_order);

			if (table == NULL) {
				i++;
				switch (bridge->driver->size_type) {
				case U8_APER_SIZE:
					bridge->current_size = A_IDX8(bridge);
					break;
				case U16_APER_SIZE:
					bridge->current_size = A_IDX16(bridge);
					break;
				case U32_APER_SIZE:
					bridge->current_size = A_IDX32(bridge);
					break;
				/* These cases will never really happen. */
				case FIXED_APER_SIZE:
				case LVL2_APER_SIZE:
				default:
					break;
				}
				temp = bridge->current_size;
			} else {
				bridge->aperture_size_idx = i;
			}
		} while (!table && (i < bridge->driver->num_aperture_sizes));
	} else {
		size = ((struct aper_size_info_fixed *) temp)->size;
		page_order = ((struct aper_size_info_fixed *) temp)->page_order;
		num_entries = ((struct aper_size_info_fixed *) temp)->num_entries;
		table = alloc_gatt_pages(page_order);
	}

	if (table == NULL)
		return -ENOMEM;

	table_end = table + ((PAGE_SIZE * (1 << page_order)) - 1);

	for (page = virt_to_page(table); page <= virt_to_page(table_end); page++)
		SetPageReserved(page);

	bridge->gatt_table_real = (u32 *) table;
	agp_gatt_table = (void *)table;

	bridge->driver->cache_flush();
#ifdef CONFIG_X86
	if (set_memory_uc((unsigned long)table, 1 << page_order))
		printk(KERN_WARNING "Could not set GATT table memory to UC!");

	bridge->gatt_table = (void *)table;
#else
	bridge->gatt_table = ioremap_nocache(virt_to_phys(table),
					(PAGE_SIZE * (1 << page_order)));
	bridge->driver->cache_flush();
#endif

	if (bridge->gatt_table == NULL) {
		for (page = virt_to_page(table); page <= virt_to_page(table_end); page++)
			ClearPageReserved(page);

		free_gatt_pages(table, page_order);

		return -ENOMEM;
	}
	bridge->gatt_bus_addr = virt_to_phys(bridge->gatt_table_real);

	/* AK: bogus, should encode addresses > 4GB */
	for (i = 0; i < num_entries; i++) {
		writel(bridge->scratch_page, bridge->gatt_table+i);
		readl(bridge->gatt_table+i);	/* PCI Posting. */
	}

	return 0;
}