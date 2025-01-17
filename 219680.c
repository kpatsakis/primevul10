static void ov518_pkt_scan(struct gspca_dev *gspca_dev,
			u8 *data,			/* isoc packet */
			int len)			/* iso packet length */
{
	struct sd *sd = (struct sd *) gspca_dev;

	/* A false positive here is likely, until OVT gives me
	 * the definitive SOF/EOF format */
	if ((!(data[0] | data[1] | data[2] | data[3] | data[5])) && data[6]) {
		ov51x_handle_button(gspca_dev, (data[6] >> 1) & 1);
		gspca_frame_add(gspca_dev, LAST_PACKET, NULL, 0);
		gspca_frame_add(gspca_dev, FIRST_PACKET, NULL, 0);
		sd->packet_nr = 0;
	}

	if (gspca_dev->last_packet_type == DISCARD_PACKET)
		return;

	/* Does this device use packet numbers ? */
	if (len & 7) {
		len--;
		if (sd->packet_nr == data[len])
			sd->packet_nr++;
		/* The last few packets of the frame (which are all 0's
		   except that they may contain part of the footer), are
		   numbered 0 */
		else if (sd->packet_nr == 0 || data[len]) {
			gspca_err(gspca_dev, "Invalid packet nr: %d (expect: %d)\n",
				  (int)data[len], (int)sd->packet_nr);
			gspca_dev->last_packet_type = DISCARD_PACKET;
			return;
		}
	}

	/* intermediate packet */
	gspca_frame_add(gspca_dev, INTER_PACKET, data, len);
}