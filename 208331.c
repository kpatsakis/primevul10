u64 acpi_os_get_timer(void)
{
	u64 time_ns = ktime_to_ns(ktime_get());
	do_div(time_ns, 100);
	return time_ns;
}