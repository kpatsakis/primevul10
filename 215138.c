void WriteJpegFile(const char * FileName)
{
    FILE * outfile;
    int a;

    if (!HaveAll){
        ErrFatal("Can't write back - didn't read all");
    }

    outfile = fopen(FileName,"wb");
    if (outfile == NULL){
        ErrFatal("Could not open file for write");
    }

    // Initial static jpeg marker.
    fputc(0xff,outfile);
    fputc(0xd8,outfile);
    
    if (Sections[0].Type != M_EXIF && Sections[0].Type != M_JFIF){
        // The image must start with an exif or jfif marker.  If we threw those away, create one.
        static uchar JfifHead[18] = {
            0xff, M_JFIF,
            0x00, 0x10, 'J' , 'F' , 'I' , 'F' , 0x00, 0x01, 
            0x01, 0x01, 0x01, 0x2C, 0x01, 0x2C, 0x00, 0x00 
        };

        if (ImageInfo.ResolutionUnit == 2 || ImageInfo.ResolutionUnit == 3){
            // Use the exif resolution info to fill out the jfif header.
            // Usually, for exif images, there's no jfif header, so if wediscard
            // the exif header, use info from the exif header for the jfif header.
            
            ImageInfo.JfifHeader.ResolutionUnits = (char)(ImageInfo.ResolutionUnit-1);
            // Jfif is 1 and 2, Exif is 2 and 3 for In and cm respecively
            ImageInfo.JfifHeader.XDensity = (int)ImageInfo.xResolution;
            ImageInfo.JfifHeader.YDensity = (int)ImageInfo.yResolution;
        }

        JfifHead[11] = ImageInfo.JfifHeader.ResolutionUnits;
        JfifHead[12] = (uchar)(ImageInfo.JfifHeader.XDensity >> 8);
        JfifHead[13] = (uchar)ImageInfo.JfifHeader.XDensity;
        JfifHead[14] = (uchar)(ImageInfo.JfifHeader.YDensity >> 8);
        JfifHead[15] = (uchar)ImageInfo.JfifHeader.YDensity;
        

        fwrite(JfifHead, 18, 1, outfile);

        // use the values from the exif data for the jfif header, if we have found values
        if (ImageInfo.ResolutionUnit != 0) { 
            // JFIF.ResolutionUnit is {1,2}, EXIF.ResolutionUnit is {2,3}
            JfifHead[11] = (uchar)ImageInfo.ResolutionUnit - 1; 
        }
        if (ImageInfo.xResolution > 0.0 && ImageInfo.yResolution > 0.0) { 
            JfifHead[12] = (uchar)((int)ImageInfo.xResolution>>8);
            JfifHead[13] = (uchar)((int)ImageInfo.xResolution);

            JfifHead[14] = (uchar)((int)ImageInfo.yResolution>>8);
            JfifHead[15] = (uchar)((int)ImageInfo.yResolution);
        }
    }


    // Write all the misc sections
    for (a=0;a<SectionsRead-1;a++){
        fputc(0xff,outfile);
        fputc((unsigned char)Sections[a].Type, outfile);
        fwrite(Sections[a].Data, Sections[a].Size, 1, outfile);
    }

    // Write the remaining image data.
    fwrite(Sections[a].Data, Sections[a].Size, 1, outfile);
       
    fclose(outfile);
}