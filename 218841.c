static const char* update_type_to_string(UINT16 updateType)
{
	if (updateType >= ARRAYSIZE(UPDATE_TYPE_STRINGS))
		return "UNKNOWN";

	return UPDATE_TYPE_STRINGS[updateType];
}