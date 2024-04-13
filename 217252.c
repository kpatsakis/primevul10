reg_write(struct pcilynx *lynx, int offset, u32 data)
{
	writel(data, lynx->registers + offset);
}