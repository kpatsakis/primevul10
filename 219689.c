static void sd_reset_snapshot(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;

	if (!sd->snapshot_needs_reset)
		return;

	/* Note it is important that we clear sd->snapshot_needs_reset,
	   before actually clearing the snapshot state in the bridge
	   otherwise we might race with the pkt_scan interrupt handler */
	sd->snapshot_needs_reset = 0;

	switch (sd->bridge) {
	case BRIDGE_OV511:
	case BRIDGE_OV511PLUS:
		reg_w(sd, R51x_SYS_SNAP, 0x02);
		reg_w(sd, R51x_SYS_SNAP, 0x00);
		break;
	case BRIDGE_OV518:
	case BRIDGE_OV518PLUS:
		reg_w(sd, R51x_SYS_SNAP, 0x02); /* Reset */
		reg_w(sd, R51x_SYS_SNAP, 0x01); /* Enable */
		break;
	case BRIDGE_OV519:
		reg_w(sd, R51x_SYS_RESET, 0x40);
		reg_w(sd, R51x_SYS_RESET, 0x00);
		break;
	}
}