static int tda9874a_getmode(struct CHIPSTATE *chip)
{
	int dsr,nsr,mode;
	int necr; /* just for debugging */

	mode = V4L2_TUNER_MODE_MONO;

	if(-1 == (dsr = chip_read2(chip,TDA9874A_DSR)))
		return mode;
	if(-1 == (nsr = chip_read2(chip,TDA9874A_NSR)))
		return mode;
	if(-1 == (necr = chip_read2(chip,TDA9874A_NECR)))
		return mode;

	/* need to store dsr/nsr somewhere */
	chip->shadow.bytes[MAXREGS-2] = dsr;
	chip->shadow.bytes[MAXREGS-1] = nsr;

	if(tda9874a_mode) {
		/* Note: DSR.RSSF and DSR.AMSTAT bits are also checked.
		 * If NICAM auto-muting is enabled, DSR.AMSTAT=1 indicates
		 * that sound has (temporarily) switched from NICAM to
		 * mono FM (or AM) on 1st sound carrier due to high NICAM bit
		 * error count. So in fact there is no stereo in this case :-(
		 * But changing the mode to V4L2_TUNER_MODE_MONO would switch
		 * external 4052 multiplexer in audio_hook().
		 */
		if(nsr & 0x02) /* NSR.S/MB=1 */
			mode |= V4L2_TUNER_MODE_STEREO;
		if(nsr & 0x01) /* NSR.D/SB=1 */
			mode |= V4L2_TUNER_MODE_LANG1 | V4L2_TUNER_MODE_LANG2;
	} else {
		if(dsr & 0x02) /* DSR.IDSTE=1 */
			mode |= V4L2_TUNER_MODE_STEREO;
		if(dsr & 0x04) /* DSR.IDDUA=1 */
			mode |= V4L2_TUNER_MODE_LANG1 | V4L2_TUNER_MODE_LANG2;
	}

	v4l_dbg(1, debug, chip->c, "tda9874a_getmode(): DSR=0x%X, NSR=0x%X, NECR=0x%X, return: %d.\n",
		 dsr, nsr, necr, mode);
	return mode;
}