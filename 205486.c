DecodeRowError(TIFF* tif, uint8* buf, tmsize_t cc, uint16 s)

{
    (void) buf;
    (void) cc;
    (void) s;

    TIFFErrorExt(tif->tif_clientdata, "TIFFReadScanline",
                 "scanline oriented access is not supported for downsampled JPEG compressed images, consider enabling TIFF_JPEGCOLORMODE as JPEGCOLORMODE_RGB." );
    return 0;
}