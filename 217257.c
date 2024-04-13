reg_set_bits(struct pcilynx *lynx, int offset, u32 mask)
{
	reg_write(lynx, offset, (reg_read(lynx, offset) | mask));
}