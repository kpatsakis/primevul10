static inline unsigned int clock_divider_to_ns(unsigned int divider)
{
	/* Period of the Rx or Tx clock in ns */
	return DIV_ROUND_CLOSEST((divider + 1) * 1000,
				 CX23888_IR_REFCLK_FREQ / 1000000);
}