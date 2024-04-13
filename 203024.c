static void agp_v3_parse_one(u32 *requested_mode, u32 *bridge_agpstat, u32 *vga_agpstat)
{
	u32 origbridge=*bridge_agpstat, origvga=*vga_agpstat;
	u32 tmp;

	if (*requested_mode & AGP3_RESERVED_MASK) {
		printk(KERN_INFO PFX "reserved bits set (%x) in mode 0x%x. Fixed.\n",
			*requested_mode & AGP3_RESERVED_MASK, *requested_mode);
		*requested_mode &= ~AGP3_RESERVED_MASK;
	}

	/* Check the speed bits make sense. */
	tmp = *requested_mode & 7;
	if (tmp == 0) {
		printk(KERN_INFO PFX "%s tried to set rate=x0. Setting to AGP3 x4 mode.\n", current->comm);
		*requested_mode |= AGPSTAT3_4X;
	}
	if (tmp >= 3) {
		printk(KERN_INFO PFX "%s tried to set rate=x%d. Setting to AGP3 x8 mode.\n", current->comm, tmp * 4);
		*requested_mode = (*requested_mode & ~7) | AGPSTAT3_8X;
	}

	/* ARQSZ - Set the value to the maximum one.
	 * Don't allow the mode register to override values. */
	*bridge_agpstat = ((*bridge_agpstat & ~AGPSTAT_ARQSZ) |
		max_t(u32,(*bridge_agpstat & AGPSTAT_ARQSZ),(*vga_agpstat & AGPSTAT_ARQSZ)));

	/* Calibration cycle.
	 * Don't allow the mode register to override values. */
	*bridge_agpstat = ((*bridge_agpstat & ~AGPSTAT_CAL_MASK) |
		min_t(u32,(*bridge_agpstat & AGPSTAT_CAL_MASK),(*vga_agpstat & AGPSTAT_CAL_MASK)));

	/* SBA *must* be supported for AGP v3 */
	*bridge_agpstat |= AGPSTAT_SBA;

	/*
	 * Set speed.
	 * Check for invalid speeds. This can happen when applications
	 * written before the AGP 3.0 standard pass AGP2.x modes to AGP3 hardware
	 */
	if (*requested_mode & AGPSTAT_MODE_3_0) {
		/*
		 * Caller hasn't a clue what it is doing. Bridge is in 3.0 mode,
		 * have been passed a 3.0 mode, but with 2.x speed bits set.
		 * AGP2.x 4x -> AGP3.0 4x.
		 */
		if (*requested_mode & AGPSTAT2_4X) {
			printk(KERN_INFO PFX "%s passes broken AGP3 flags (%x). Fixed.\n",
						current->comm, *requested_mode);
			*requested_mode &= ~AGPSTAT2_4X;
			*requested_mode |= AGPSTAT3_4X;
		}
	} else {
		/*
		 * The caller doesn't know what they are doing. We are in 3.0 mode,
		 * but have been passed an AGP 2.x mode.
		 * Convert AGP 1x,2x,4x -> AGP 3.0 4x.
		 */
		printk(KERN_INFO PFX "%s passes broken AGP2 flags (%x) in AGP3 mode. Fixed.\n",
					current->comm, *requested_mode);
		*requested_mode &= ~(AGPSTAT2_4X | AGPSTAT2_2X | AGPSTAT2_1X);
		*requested_mode |= AGPSTAT3_4X;
	}

	if (*requested_mode & AGPSTAT3_8X) {
		if (!(*bridge_agpstat & AGPSTAT3_8X)) {
			*bridge_agpstat &= ~(AGPSTAT3_8X | AGPSTAT3_RSVD);
			*bridge_agpstat |= AGPSTAT3_4X;
			printk(KERN_INFO PFX "%s requested AGPx8 but bridge not capable.\n", current->comm);
			return;
		}
		if (!(*vga_agpstat & AGPSTAT3_8X)) {
			*bridge_agpstat &= ~(AGPSTAT3_8X | AGPSTAT3_RSVD);
			*bridge_agpstat |= AGPSTAT3_4X;
			printk(KERN_INFO PFX "%s requested AGPx8 but graphic card not capable.\n", current->comm);
			return;
		}
		/* All set, bridge & device can do AGP x8*/
		*bridge_agpstat &= ~(AGPSTAT3_4X | AGPSTAT3_RSVD);
		goto done;

	} else if (*requested_mode & AGPSTAT3_4X) {
		*bridge_agpstat &= ~(AGPSTAT3_8X | AGPSTAT3_RSVD);
		*bridge_agpstat |= AGPSTAT3_4X;
		goto done;

	} else {

		/*
		 * If we didn't specify an AGP mode, we see if both
		 * the graphics card, and the bridge can do x8, and use if so.
		 * If not, we fall back to x4 mode.
		 */
		if ((*bridge_agpstat & AGPSTAT3_8X) && (*vga_agpstat & AGPSTAT3_8X)) {
			printk(KERN_INFO PFX "No AGP mode specified. Setting to highest mode "
				"supported by bridge & card (x8).\n");
			*bridge_agpstat &= ~(AGPSTAT3_4X | AGPSTAT3_RSVD);
			*vga_agpstat &= ~(AGPSTAT3_4X | AGPSTAT3_RSVD);
		} else {
			printk(KERN_INFO PFX "Fell back to AGPx4 mode because");
			if (!(*bridge_agpstat & AGPSTAT3_8X)) {
				printk(KERN_INFO PFX "bridge couldn't do x8. bridge_agpstat:%x (orig=%x)\n",
					*bridge_agpstat, origbridge);
				*bridge_agpstat &= ~(AGPSTAT3_8X | AGPSTAT3_RSVD);
				*bridge_agpstat |= AGPSTAT3_4X;
			}
			if (!(*vga_agpstat & AGPSTAT3_8X)) {
				printk(KERN_INFO PFX "graphics card couldn't do x8. vga_agpstat:%x (orig=%x)\n",
					*vga_agpstat, origvga);
				*vga_agpstat &= ~(AGPSTAT3_8X | AGPSTAT3_RSVD);
				*vga_agpstat |= AGPSTAT3_4X;
			}
		}
	}

done:
	/* Apply any errata. */
	if (agp_bridge->flags & AGP_ERRATA_FASTWRITES)
		*bridge_agpstat &= ~AGPSTAT_FW;

	if (agp_bridge->flags & AGP_ERRATA_SBA)
		*bridge_agpstat &= ~AGPSTAT_SBA;

	if (agp_bridge->flags & AGP_ERRATA_1X) {
		*bridge_agpstat &= ~(AGPSTAT2_2X | AGPSTAT2_4X);
		*bridge_agpstat |= AGPSTAT2_1X;
	}
}