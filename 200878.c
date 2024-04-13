static int tda9874a_initialize(struct CHIPSTATE *chip)
{
	if (tda9874a_SIF > 2)
		tda9874a_SIF = 1;
	if (tda9874a_STD >= ARRAY_SIZE(tda9874a_modelist))
		tda9874a_STD = 0;
	if(tda9874a_AMSEL > 1)
		tda9874a_AMSEL = 0;

	if(tda9874a_SIF == 1)
		tda9874a_GCONR = 0xc0;	/* sound IF input 1 */
	else
		tda9874a_GCONR = 0xc1;	/* sound IF input 2 */

	tda9874a_ESP = tda9874a_STD;
	tda9874a_mode = (tda9874a_STD < 5) ? 0 : 1;

	if(tda9874a_AMSEL == 0)
		tda9874a_NCONR = 0x01; /* auto-mute: analog mono input */
	else
		tda9874a_NCONR = 0x05; /* auto-mute: 1st carrier FM or AM */

	tda9874a_setup(chip);
	return 0;
}