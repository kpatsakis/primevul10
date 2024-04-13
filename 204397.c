void sctp_ulpq_renege(struct sctp_ulpq *ulpq, struct sctp_chunk *chunk,
		      gfp_t gfp)
{
	struct sctp_association *asoc;
	__u16 needed, freed;

	asoc = ulpq->asoc;

	if (chunk) {
		needed = ntohs(chunk->chunk_hdr->length);
		needed -= sizeof(sctp_data_chunk_t);
	} else 
		needed = SCTP_DEFAULT_MAXWINDOW;

	freed = 0;

	if (skb_queue_empty(&asoc->base.sk->sk_receive_queue)) {
		freed = sctp_ulpq_renege_order(ulpq, needed);
		if (freed < needed) {
			freed += sctp_ulpq_renege_frags(ulpq, needed - freed);
		}
	}
	/* If able to free enough room, accept this chunk. */
	if (chunk && (freed >= needed)) {
		__u32 tsn;
		tsn = ntohl(chunk->subh.data_hdr->tsn);
		sctp_tsnmap_mark(&asoc->peer.tsn_map, tsn);
		sctp_ulpq_tail_data(ulpq, chunk, gfp);
		
		sctp_ulpq_partial_delivery(ulpq, chunk, gfp);
	}

	return;
}