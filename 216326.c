void freeServiceTable(service_table *table)
{
	ixmlFreeDOMString(table->URLBase);
	freeServiceList(table->serviceList);
	table->serviceList = NULL;
	table->endServiceList = NULL;
}