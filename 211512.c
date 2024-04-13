exif_process_SOFn(unsigned char* Data, int /*marker*/, jpeg_sof_info* result) {
  result->bits_per_sample = Data[2];
  result->height          = php_jpg_get16(Data+3);
  result->width           = php_jpg_get16(Data+5);
  result->num_components  = Data[7];
}