  void moduleInit() override {
    HHVM_FE(exif_imagetype);
    HHVM_FE(exif_read_data);
    HHVM_FE(exif_tagname);
    HHVM_FE(exif_thumbnail);

    loadSystemlib();
  }