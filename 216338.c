void printServiceTable(
	service_table *table, Upnp_LogLevel level, Dbg_Module module)
{
	UpnpPrintf(level,
		module,
		__FILE__,
		__LINE__,
		"URL_BASE: %s\n",
		table->URLBase);
	UpnpPrintf(level, module, __FILE__, __LINE__, "Services: \n");
	printServiceList(table->serviceList, level, module);
}