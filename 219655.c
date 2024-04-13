static void ov519_set_fr(struct sd *sd)
{
	int fr;
	u8 clock;
	/* frame rate table with indices:
	 *	- mode = 0: 320x240, 1: 640x480
	 *	- fr rate = 0: 30, 1: 25, 2: 20, 3: 15, 4: 10, 5: 5
	 *	- reg = 0: bridge a4, 1: bridge 23, 2: sensor 11 (clock)
	 */
	static const u8 fr_tb[2][6][3] = {
		{{0x04, 0xff, 0x00},
		 {0x04, 0x1f, 0x00},
		 {0x04, 0x1b, 0x00},
		 {0x04, 0x15, 0x00},
		 {0x04, 0x09, 0x00},
		 {0x04, 0x01, 0x00}},
		{{0x0c, 0xff, 0x00},
		 {0x0c, 0x1f, 0x00},
		 {0x0c, 0x1b, 0x00},
		 {0x04, 0xff, 0x01},
		 {0x04, 0x1f, 0x01},
		 {0x04, 0x1b, 0x01}},
	};

	if (frame_rate > 0)
		sd->frame_rate = frame_rate;
	if (sd->frame_rate >= 30)
		fr = 0;
	else if (sd->frame_rate >= 25)
		fr = 1;
	else if (sd->frame_rate >= 20)
		fr = 2;
	else if (sd->frame_rate >= 15)
		fr = 3;
	else if (sd->frame_rate >= 10)
		fr = 4;
	else
		fr = 5;
	reg_w(sd, 0xa4, fr_tb[sd->gspca_dev.curr_mode][fr][0]);
	reg_w(sd, 0x23, fr_tb[sd->gspca_dev.curr_mode][fr][1]);
	clock = fr_tb[sd->gspca_dev.curr_mode][fr][2];
	if (sd->sensor == SEN_OV7660)
		clock |= 0x80;		/* enable double clock */
	ov518_i2c_w(sd, OV7670_R11_CLKRC, clock);
}