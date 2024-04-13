struct agp_bridge_data *agp_generic_find_bridge(struct pci_dev *pdev)
{
	if (list_empty(&agp_bridges))
		return NULL;

	return agp_bridge;
}