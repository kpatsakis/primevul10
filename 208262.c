acpi_os_write_memory(acpi_physical_address phys_addr, u64 value, u32 width)
{
	void __iomem *virt_addr;
	unsigned int size = width / 8;
	bool unmap = false;

	rcu_read_lock();
	virt_addr = acpi_map_vaddr_lookup(phys_addr, size);
	if (!virt_addr) {
		rcu_read_unlock();
		virt_addr = acpi_os_ioremap(phys_addr, size);
		if (!virt_addr)
			return AE_BAD_ADDRESS;
		unmap = true;
	}

	switch (width) {
	case 8:
		writeb(value, virt_addr);
		break;
	case 16:
		writew(value, virt_addr);
		break;
	case 32:
		writel(value, virt_addr);
		break;
	case 64:
		writeq(value, virt_addr);
		break;
	default:
		BUG();
	}

	if (unmap)
		iounmap(virt_addr);
	else
		rcu_read_unlock();

	return AE_OK;
}