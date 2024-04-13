packet_irq_handler(struct pcilynx *lynx)
{
	struct client *client;
	u32 tcode_mask, tcode, timestamp;
	size_t length;
	struct timespec64 ts64;

	/* FIXME: Also report rcv_speed. */

	length = __le32_to_cpu(lynx->rcv_pcl->pcl_status) & 0x00001fff;
	tcode  = __le32_to_cpu(lynx->rcv_buffer[1]) >> 4 & 0xf;

	ktime_get_real_ts64(&ts64);
	timestamp = ts64.tv_nsec / NSEC_PER_USEC;
	lynx->rcv_buffer[0] = (__force __le32)timestamp;

	if (length == PHY_PACKET_SIZE)
		tcode_mask = 1 << TCODE_PHY_PACKET;
	else
		tcode_mask = 1 << tcode;

	spin_lock(&lynx->client_list_lock);

	list_for_each_entry(client, &lynx->client_list, link)
		if (client->tcode_mask & tcode_mask)
			packet_buffer_put(&client->buffer,
					  lynx->rcv_buffer, length + 4);

	spin_unlock(&lynx->client_list_lock);
}