static void ov_hires_configure(struct sd *sd)
{
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;
	int high, low;

	if (sd->bridge != BRIDGE_OVFX2) {
		gspca_err(gspca_dev, "error hires sensors only supported with ovfx2\n");
		return;
	}

	gspca_dbg(gspca_dev, D_PROBE, "starting ov hires configuration\n");

	/* Detect sensor (sub)type */
	high = i2c_r(sd, 0x0a);
	low = i2c_r(sd, 0x0b);
	/* info("%x, %x", high, low); */
	switch (high) {
	case 0x96:
		switch (low) {
		case 0x40:
			gspca_dbg(gspca_dev, D_PROBE, "Sensor is a OV2610\n");
			sd->sensor = SEN_OV2610;
			return;
		case 0x41:
			gspca_dbg(gspca_dev, D_PROBE, "Sensor is a OV2610AE\n");
			sd->sensor = SEN_OV2610AE;
			return;
		case 0xb1:
			gspca_dbg(gspca_dev, D_PROBE, "Sensor is a OV9600\n");
			sd->sensor = SEN_OV9600;
			return;
		}
		break;
	case 0x36:
		if ((low & 0x0f) == 0x00) {
			gspca_dbg(gspca_dev, D_PROBE, "Sensor is a OV3610\n");
			sd->sensor = SEN_OV3610;
			return;
		}
		break;
	}
	gspca_err(gspca_dev, "Error unknown sensor type: %02x%02x\n",
		  high, low);
}