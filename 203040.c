static void agp_v2_parse_one(u32 *requested_mode, u32 *bridge_agpstat, u32 *vga_agpstat)
{
	u32 tmp;

	if (*requested_mode & AGP2_RESERVED_MASK) {
		printk(KERN_INFO PFX "reserved bits set (%x) in mode 0x%x. Fixed.\n",
			*requested_mode & AGP2_RESERVED_MASK, *requested_mode);
		*requested_mode &= ~AGP2_RESERVED_MASK;
	}

	/*
	 * Some dumb bridges are programmed to disobey the AGP2 spec.
	 * This is likely a BIOS misprogramming rather than poweron default, or
	 * it would be a lot more common.
	 * https://bugs.freedesktop.org/show_bug.cgi?id=8816
	 * AGPv2 spec 6.1.9 states:
	 *   The RATE field indicates the data transfer rates supported by this
	 *   device. A.G.P. devices must report all that apply.
	 * Fix them up as best we can.
	 */
	switch (*bridge_agpstat & 7) {
	case 4:
		*bridge_agpstat |= (AGPSTAT2_2X | AGPSTAT2_1X);
		printk(KERN_INFO PFX "BIOS bug. AGP bridge claims to only support x4 rate"
			"Fixing up support for x2 & x1\n");
		break;
	case 2:
		*bridge_agpstat |= AGPSTAT2_1X;
		printk(KERN_INFO PFX "BIOS bug. AGP bridge claims to only support x2 rate"
			"Fixing up support for x1\n");
		break;
	default:
		break;
	}

	/* Check the speed bits make sense. Only one should be set. */
	tmp = *requested_mode & 7;
	switch (tmp) {
		case 0:
			printk(KERN_INFO PFX "%s tried to set rate=x0. Setting to x1 mode.\n", current->comm);
			*requested_mode |= AGPSTAT2_1X;
			break;
		case 1:
		case 2:
			break;
		case 3:
			*requested_mode &= ~(AGPSTAT2_1X);	/* rate=2 */
			break;
		case 4:
			break;
		case 5:
		case 6:
		case 7:
			*requested_mode &= ~(AGPSTAT2_1X|AGPSTAT2_2X); /* rate=4*/
			break;
	}

	/* disable SBA if it's not supported */
	if (!((*bridge_agpstat & AGPSTAT_SBA) && (*vga_agpstat & AGPSTAT_SBA) && (*requested_mode & AGPSTAT_SBA)))
		*bridge_agpstat &= ~AGPSTAT_SBA;

	/* Set rate */
	if (!((*bridge_agpstat & AGPSTAT2_4X) && (*vga_agpstat & AGPSTAT2_4X) && (*requested_mode & AGPSTAT2_4X)))
		*bridge_agpstat &= ~AGPSTAT2_4X;

	if (!((*bridge_agpstat & AGPSTAT2_2X) && (*vga_agpstat & AGPSTAT2_2X) && (*requested_mode & AGPSTAT2_2X)))
		*bridge_agpstat &= ~AGPSTAT2_2X;

	if (!((*bridge_agpstat & AGPSTAT2_1X) && (*vga_agpstat & AGPSTAT2_1X) && (*requested_mode & AGPSTAT2_1X)))
		*bridge_agpstat &= ~AGPSTAT2_1X;

	/* Now we know what mode it should be, clear out the unwanted bits. */
	if (*bridge_agpstat & AGPSTAT2_4X)
		*bridge_agpstat &= ~(AGPSTAT2_1X | AGPSTAT2_2X);	/* 4X */

	if (*bridge_agpstat & AGPSTAT2_2X)
		*bridge_agpstat &= ~(AGPSTAT2_1X | AGPSTAT2_4X);	/* 2X */

	if (*bridge_agpstat & AGPSTAT2_1X)
		*bridge_agpstat &= ~(AGPSTAT2_2X | AGPSTAT2_4X);	/* 1X */

	/* Apply any errata. */
	if (agp_bridge->flags & AGP_ERRATA_FASTWRITES)
		*bridge_agpstat &= ~AGPSTAT_FW;

	if (agp_bridge->flags & AGP_ERRATA_SBA)
		*bridge_agpstat &= ~AGPSTAT_SBA;

	if (agp_bridge->flags & AGP_ERRATA_1X) {
		*bridge_agpstat &= ~(AGPSTAT2_2X | AGPSTAT2_4X);
		*bridge_agpstat |= AGPSTAT2_1X;
	}

	/* If we've dropped down to 1X, disable fast writes. */
	if (*bridge_agpstat & AGPSTAT2_1X)
		*bridge_agpstat &= ~AGPSTAT_FW;
}