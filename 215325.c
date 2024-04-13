iasecc_sdo_tag_from_class(unsigned sdo_class)
{
	switch (sdo_class & ~IASECC_OBJECT_REF_LOCAL)   {
	case IASECC_SDO_CLASS_CHV:
		return IASECC_SDO_CHV_TAG;
	case IASECC_SDO_CLASS_RSA_PRIVATE:
		return IASECC_SDO_PRVKEY_TAG;
	case IASECC_SDO_CLASS_RSA_PUBLIC:
		return IASECC_SDO_PUBKEY_TAG;
	case IASECC_SDO_CLASS_SE:
		return IASECC_SDO_CLASS_SE;
	case IASECC_SDO_CLASS_KEYSET:
		return IASECC_SDO_KEYSET_TAG;
	}

	return -1;
}