static void ovfx2_pkt_scan(struct gspca_dev *gspca_dev,
			u8 *data,			/* isoc packet */
			int len)			/* iso packet length */
{
	struct sd *sd = (struct sd *) gspca_dev;

	gspca_frame_add(gspca_dev, INTER_PACKET, data, len);

	/* A short read signals EOF */
	if (len < gspca_dev->cam.bulk_size) {
		/* If the frame is short, and it is one of the first ones
		   the sensor and bridge are still syncing, so drop it. */
		if (sd->first_frame) {
			sd->first_frame--;
			if (gspca_dev->image_len <
				  sd->gspca_dev.pixfmt.width *
					sd->gspca_dev.pixfmt.height)
				gspca_dev->last_packet_type = DISCARD_PACKET;
		}
		gspca_frame_add(gspca_dev, LAST_PACKET, NULL, 0);
		gspca_frame_add(gspca_dev, FIRST_PACKET, NULL, 0);
	}
}