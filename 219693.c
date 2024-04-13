static void sd_pkt_scan(struct gspca_dev *gspca_dev,
			u8 *data,			/* isoc packet */
			int len)			/* iso packet length */
{
	struct sd *sd = (struct sd *) gspca_dev;

	switch (sd->bridge) {
	case BRIDGE_OV511:
	case BRIDGE_OV511PLUS:
		ov511_pkt_scan(gspca_dev, data, len);
		break;
	case BRIDGE_OV518:
	case BRIDGE_OV518PLUS:
		ov518_pkt_scan(gspca_dev, data, len);
		break;
	case BRIDGE_OV519:
		ov519_pkt_scan(gspca_dev, data, len);
		break;
	case BRIDGE_OVFX2:
		ovfx2_pkt_scan(gspca_dev, data, len);
		break;
	case BRIDGE_W9968CF:
		w9968cf_pkt_scan(gspca_dev, data, len);
		break;
	}
}