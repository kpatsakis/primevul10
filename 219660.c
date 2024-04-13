static void ov6xx0_configure(struct sd *sd)
{
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;
	int rc;

	gspca_dbg(gspca_dev, D_PROBE, "starting OV6xx0 configuration\n");

	/* Detect sensor (sub)type */
	rc = i2c_r(sd, OV7610_REG_COM_I);
	if (rc < 0) {
		gspca_err(gspca_dev, "Error detecting sensor type\n");
		return;
	}

	/* Ugh. The first two bits are the version bits, but
	 * the entire register value must be used. I guess OVT
	 * underestimated how many variants they would make. */
	switch (rc) {
	case 0x00:
		sd->sensor = SEN_OV6630;
		pr_warn("WARNING: Sensor is an OV66308. Your camera may have been misdetected in previous driver versions.\n");
		break;
	case 0x01:
		sd->sensor = SEN_OV6620;
		gspca_dbg(gspca_dev, D_PROBE, "Sensor is an OV6620\n");
		break;
	case 0x02:
		sd->sensor = SEN_OV6630;
		gspca_dbg(gspca_dev, D_PROBE, "Sensor is an OV66308AE\n");
		break;
	case 0x03:
		sd->sensor = SEN_OV66308AF;
		gspca_dbg(gspca_dev, D_PROBE, "Sensor is an OV66308AF\n");
		break;
	case 0x90:
		sd->sensor = SEN_OV6630;
		pr_warn("WARNING: Sensor is an OV66307. Your camera may have been misdetected in previous driver versions.\n");
		break;
	default:
		gspca_err(gspca_dev, "FATAL: Unknown sensor version: 0x%02x\n",
			  rc);
		return;
	}

	/* Set sensor-specific vars */
	sd->sif = 1;
}