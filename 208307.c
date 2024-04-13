static u8 __init acpi_table_checksum(u8 *buffer, u32 length)
{
	u8 sum = 0;
	u8 *end = buffer + length;

	while (buffer < end)
		sum = (u8) (sum + *(buffer++));
	return sum;
}