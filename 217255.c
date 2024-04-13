set_phy_reg(struct pcilynx *lynx, int addr, int val)
{
	if (addr > 15) {
		dev_err(&lynx->pci_device->dev,
			"PHY register address %d out of range\n", addr);
		return -1;
	}
	if (val > 0xff) {
		dev_err(&lynx->pci_device->dev,
			"PHY register value %d out of range\n", val);
		return -1;
	}
	reg_write(lynx, LINK_PHY, LINK_PHY_WRITE |
		  LINK_PHY_ADDR(addr) | LINK_PHY_WDATA(val));

	return 0;
}