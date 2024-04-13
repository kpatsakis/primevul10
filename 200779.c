void sctp_assoc_rwnd_decrease(struct sctp_association *asoc, unsigned len)
{
	SCTP_ASSERT(asoc->rwnd, "rwnd zero", return);
	SCTP_ASSERT(!asoc->rwnd_over, "rwnd_over not zero", return);
	if (asoc->rwnd >= len) {
		asoc->rwnd -= len;
	} else {
		asoc->rwnd_over = len - asoc->rwnd;
		asoc->rwnd = 0;
	}
	SCTP_DEBUG_PRINTK("%s: asoc %p rwnd decreased by %d to (%u, %u)\n",
			  __func__, asoc, len, asoc->rwnd,
			  asoc->rwnd_over);
}