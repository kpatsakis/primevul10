static inline void ov51x_restart(struct sd *sd)
{
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;

	gspca_dbg(gspca_dev, D_STREAM, "restarting\n");
	if (!sd->stopped)
		return;
	sd->stopped = 0;

	/* Reinitialize the stream */
	switch (sd->bridge) {
	case BRIDGE_OV511:
	case BRIDGE_OV511PLUS:
		reg_w(sd, R51x_SYS_RESET, 0x00);
		break;
	case BRIDGE_OV518:
	case BRIDGE_OV518PLUS:
		reg_w(sd, 0x2f, 0x80);
		reg_w(sd, R51x_SYS_RESET, 0x00);
		break;
	case BRIDGE_OV519:
		reg_w(sd, OV519_R51_RESET1, 0x0f);
		reg_w(sd, OV519_R51_RESET1, 0x00);
		reg_w(sd, 0x22, 0x1d);		/* FRAR */
		break;
	case BRIDGE_OVFX2:
		reg_w_mask(sd, 0x0f, 0x02, 0x02);
		break;
	case BRIDGE_W9968CF:
		reg_w(sd, 0x3c, 0x8a05); /* USB FIFO enable */
		break;
	}
}