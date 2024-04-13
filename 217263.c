reg_read(struct pcilynx *lynx, int offset)
{
	return readl(lynx->registers + offset);
}