static inline unsigned long next_tid(unsigned long tid)
{
	return tid + TID_STEP;
}