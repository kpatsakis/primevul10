static void chip_thread_wake(unsigned long data)
{
	struct CHIPSTATE *chip = (struct CHIPSTATE*)data;
	wake_up_process(chip->thread);
}