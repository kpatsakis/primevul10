static int detect_unicard(
	sc_pkcs15_card_t *p15card
){
	if(!insert_cert(p15card,"41004352", 0x45, 1, "Zertifikat 1")){
		set_string(&p15card->tokeninfo->manufacturer_id, "JLU Giessen");
		set_string(&p15card->tokeninfo->label, "JLU Giessen Card");

		insert_cert(p15card,"41004353", 0x46, 1, "Zertifikat 2");
		insert_cert(p15card,"41004354", 0x47, 1, "Zertifikat 3");
		insert_key(p15card,"41005103", 0x45, 0x83, 1024, 1, "Schluessel 1");
		insert_key(p15card,"41005104", 0x46, 0x84, 1024, 1, "Schluessel 2");
		insert_key(p15card,"41005105", 0x47, 0x85, 1024, 1, "Schluessel 3");

	} else if(!insert_cert(p15card,"41014352", 0x45, 1, "Zertifikat 1")){
		set_string(&p15card->tokeninfo->manufacturer_id, "TU Darmstadt");
		set_string(&p15card->tokeninfo->label, "TUD Card");

		insert_cert(p15card,"41014353", 0x46, 1, "Zertifikat 2");
		insert_cert(p15card,"41014354", 0x47, 1, "Zertifikat 3");
		insert_key(p15card,"41015103", 0x45, 0x83, 1024, 1, "Schluessel 1");
		insert_key(p15card,"41015104", 0x46, 0x84, 1024, 1, "Schluessel 2");
		insert_key(p15card,"41015105", 0x47, 0x85, 1024, 1, "Schluessel 3");

	} else return 1;

	insert_pin(p15card,"5000", 1, 2, 0x00, 6, "PIN",
		SC_PKCS15_PIN_FLAG_CASE_SENSITIVE | SC_PKCS15_PIN_FLAG_INITIALIZED
	);
	insert_pin(p15card,"5008", 2, 0, 0x01, 8, "PUK",
		SC_PKCS15_PIN_FLAG_CASE_SENSITIVE | SC_PKCS15_PIN_FLAG_INITIALIZED |
		SC_PKCS15_PIN_FLAG_UNBLOCKING_PIN | SC_PKCS15_PIN_FLAG_SO_PIN
	);

	return 0;
}