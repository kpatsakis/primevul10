static inline unsigned int tid_to_cpu(unsigned long tid)
{
	return tid % TID_STEP;
}