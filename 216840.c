void MessageWrapper::ReadConfig(const char* prefixname, const char* suffixname, const char* fixedname)
{
	ConfigTag* tag = ServerInstance->Config->ConfValue("options");
	prefix = tag->getString(fixedname);
	fixed = (!prefix.empty());
	if (!fixed)
	{
		prefix = tag->getString(prefixname);
		suffix = tag->getString(suffixname);
	}
}