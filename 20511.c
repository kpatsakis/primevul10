static inline u16 freq_to_clock_divider(unsigned int freq,
					unsigned int rollovers)
{
	return count_to_clock_divider(
		   DIV_ROUND_CLOSEST(CX23888_IR_REFCLK_FREQ, freq * rollovers));
}