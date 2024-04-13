static inline unsigned long tid_to_event(unsigned long tid)
{
	return tid / TID_STEP;
}