static void skip_bits(struct rar5* rar, int bits) {
	const int new_bits = rar->bits.bit_addr + bits;
	rar->bits.in_addr += new_bits >> 3;
	rar->bits.bit_addr = new_bits & 7;
}