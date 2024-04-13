DECLAREcpFunc(cpBiasedContig2Contig)
{
	if (spp == 1) {
		tsize_t biasSize = TIFFScanlineSize(bias);
		tsize_t bufSize = TIFFScanlineSize(in);
		tdata_t buf, biasBuf;
		uint32 biasWidth = 0, biasLength = 0;
		TIFFGetField(bias, TIFFTAG_IMAGEWIDTH, &biasWidth);
		TIFFGetField(bias, TIFFTAG_IMAGELENGTH, &biasLength);
		if (biasSize == bufSize &&
		    imagelength == biasLength && imagewidth == biasWidth) {
			uint16 sampleBits = 0;
			biasFn *subtractLine;
			TIFFGetField(in, TIFFTAG_BITSPERSAMPLE, &sampleBits);
			subtractLine = lineSubtractFn (sampleBits);
			if (subtractLine) {
				uint32 row;
				buf = _TIFFmalloc(bufSize);
				biasBuf = _TIFFmalloc(bufSize);
				for (row = 0; row < imagelength; row++) {
					if (TIFFReadScanline(in, buf, row, 0) < 0
					    && !ignore) {
						TIFFError(TIFFFileName(in),
						    "Error, can't read scanline %lu",
						    (unsigned long) row);
						goto bad;
					}
					if (TIFFReadScanline(bias, biasBuf, row, 0) < 0
					    && !ignore) {
						TIFFError(TIFFFileName(in),
						    "Error, can't read biased scanline %lu",
						    (unsigned long) row);
						goto bad;
					}
					subtractLine (buf, biasBuf, imagewidth);
					if (TIFFWriteScanline(out, buf, row, 0) < 0) {
						TIFFError(TIFFFileName(out),
						    "Error, can't write scanline %lu",
						    (unsigned long) row);
						goto bad;
					}
				}

				_TIFFfree(buf);
				_TIFFfree(biasBuf);
				TIFFSetDirectory(bias,
				    TIFFCurrentDirectory(bias)); /* rewind */
				return 1;
bad:
				_TIFFfree(buf);
				_TIFFfree(biasBuf);
				return 0;
			} else {
				TIFFError(TIFFFileName(in),
				    "No support for biasing %d bit pixels\n",
				    sampleBits);
				return 0;
			}
		}
		TIFFError(TIFFFileName(in),
		    "Bias image %s,%d\nis not the same size as %s,%d\n",
		    TIFFFileName(bias), TIFFCurrentDirectory(bias),
		    TIFFFileName(in), TIFFCurrentDirectory(in));
		return 0;
	} else {
		TIFFError(TIFFFileName(in),
		    "Can't bias %s,%d as it has >1 Sample/Pixel\n",
		    TIFFFileName(in), TIFFCurrentDirectory(in));
		return 0;
	}

}