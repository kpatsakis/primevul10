static inline u16 ns_to_clock_divider(unsigned int ns)
{
	return count_to_clock_divider(
		DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ / 1000000 * ns, 1000));
}