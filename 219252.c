yaffs_initialize_spare_format(YAFFSFS_INFO * yfs, TSK_OFF_T maxBlocksToTest){

    // Testing parameters - can all be changed
    unsigned int blocksToTest = 10;  // Number of blocks (64 chunks) to test
    unsigned int chunksToTest = 10;  // Number of chunks to test in each block 
    unsigned int minChunksRead = 10; // Minimum number of chunks we require to run the test (we might not get the full number we want to test for a very small file)

    unsigned int chunkSize = yfs->page_size + yfs->spare_size;
    unsigned int blockSize = yfs->chunks_per_block * chunkSize;

    TSK_FS_INFO *fs = &(yfs->fs_info);
    unsigned char *spareBuffer;

    unsigned int blockIndex;
    unsigned int chunkIndex;

    unsigned int currentOffset;

    unsigned char * allSpares;
    unsigned int allSparesLength;
    
    TSK_OFF_T maxBlocks;

    bool skipBlock;
    int goodOffset;
    unsigned int nGoodSpares;
    unsigned int nBlocksTested;

    int okOffsetFound = 0;   // Used as a flag for if we've found an offset that sort of works but doesn't seem great
    int goodOffsetFound = 0; // Flag to mark that we've found an offset that also passed secondary testing
    int bestOffset = 0;

    bool allSameByte; // Used in test that the spare area fields not be one repeated byte

    unsigned int i;

    int thisChunkBase;
    int lastChunkBase;

    // The spare area needs to be at least 16 bytes to run the test
    if(yfs->spare_size < 16){
        if(tsk_verbose && (! yfs->autoDetect)){
            tsk_fprintf(stderr,
                "yaffs_initialize_spare_format failed - given spare size (%d) is not large enough to contain needed fields\n", yfs->spare_size);
        }
        return TSK_ERR;
    }

    if ((spareBuffer = (unsigned char*) tsk_malloc(yfs->spare_size)) == NULL) {
        return TSK_ERR;
    }

    allSparesLength = yfs->spare_size * blocksToTest * chunksToTest;
    if ((allSpares = (unsigned char*) tsk_malloc(allSparesLength)) == NULL) {
        free(spareBuffer);
        return TSK_ERR;
    }

    // Initialize the pointers to one of the configurations we've seen (thought these defaults should not get used)
    yfs->spare_seq_offset = 0;
    yfs->spare_obj_id_offset = 4;
    yfs->spare_chunk_id_offset = 8;
    yfs->spare_nbytes_offset = 12;

    // Assume the data we want is 16 consecutive bytes in the order:
    //  seq num, obj id, chunk id, byte count
    //  (not sure we're guaranteed this but we wouldn't be able to deal with the alternative anyway)
    // Seq num is the important one. This number is constant in each block (block = 64 chunks), meaning
    //  all chunks in a block will share the same sequence number. The YAFFS2 descriptions would seem to
    //  indicate it should be different for each block, but this doesn't seem to always be the case.
    //  In particular we frequently see the 0x1000 seq number used over multiple blocks, but this isn't the only
    //  observed exception.

    // Calculate the number of blocks in the image
    maxBlocks = yfs->fs_info.img_info->size / (yfs->chunks_per_block * chunkSize);

    // If maxBlocksToTest = 0 (unlimited), set it to the total number of blocks
    // Also reduce the number of blocks to test if it is larger than the total number of blocks
    if ((maxBlocksToTest == 0) || (maxBlocksToTest > maxBlocks)){
        maxBlocksToTest = maxBlocks;
    }

    nGoodSpares = 0;
    nBlocksTested = 0;
    for (TSK_OFF_T blockIndex = 0;blockIndex < maxBlocksToTest;blockIndex++){

        // Read the last spare area that we want to test first
        TSK_OFF_T offset = (TSK_OFF_T)blockIndex * blockSize + (chunksToTest - 1) * chunkSize + yfs->page_size;
        ssize_t cnt = tsk_img_read(fs->img_info, offset, (char *) spareBuffer,
            yfs->spare_size);
        if ((cnt < 0) || ((unsigned int)cnt < yfs->spare_size)) {
            break;
        }

        // Is the spare all 0xff / 0x00?
        // If not, we know we should have all allocated chunks since YAFFS2 writes sequentially in a block
        //  - can't have an unallocated chunk followed by an allocated one
        // We occasionally see almost all null spare area with a few 0xff, which is not a valid spare.
        skipBlock = true;
        for (i = 0;i < yfs->spare_size;i++){
            if((spareBuffer[i] != 0xff) && (spareBuffer[i] != 0x00)){
                skipBlock = false;
                break;
            }
        }

        if (skipBlock){
            continue;
        }

        // If this block is potentialy valid (i.e., the spare contains something besides 0x00 and 0xff), copy all the spares into
        // the big array of extracted spare areas

        // Copy this spare area
        nGoodSpares++;
        for (i = 0;i < yfs->spare_size;i++){
            allSpares[nBlocksTested * yfs->spare_size * chunksToTest + (chunksToTest - 1) * yfs->spare_size + i] = spareBuffer[i];
        }

        // Copy all earlier spare areas in the block
        for (chunkIndex = 0;chunkIndex < chunksToTest - 1;chunkIndex++){
            offset = blockIndex * blockSize + chunkIndex * chunkSize + yfs->page_size;
            cnt = tsk_img_read(fs->img_info, offset, (char *) spareBuffer,
                yfs->spare_size);
            if ((cnt < 0) || ((unsigned int)cnt < yfs->spare_size)) {
                // We really shouldn't run out of data here since we already read in the furthest entry
                break; // Break out of chunksToTest loop
            }

            nGoodSpares++;
            for(i = 0;i < yfs->spare_size;i++){
                allSpares[nBlocksTested * yfs->spare_size * chunksToTest + chunkIndex * yfs->spare_size + i] = spareBuffer[i];
            }
        }

        // Record that we've found a potentially valid block
        nBlocksTested++;

        // If we've found enough potentailly valid blocks, break
        if (nBlocksTested >= blocksToTest){
            break;
        }
    }

    // Make sure we read enough data to reasonably perform the testing
    if (nGoodSpares < minChunksRead){

        if (tsk_verbose && (! yfs->autoDetect)){
            tsk_fprintf(stderr,
                "yaffs_initialize_spare_format failed - not enough potentially valid data could be read\n");
        }

        free(spareBuffer);
        free(allSpares);
        return TSK_ERR;
    }

    if (tsk_verbose && (! yfs->autoDetect)){
        tsk_fprintf(stderr,
            "yaffs_initialize_spare_format: Testing potential offsets for the sequence number in the spare area\n");
    }

    // Print out the collected spare areas if we're in verbose mode
    if(tsk_verbose && (! yfs->autoDetect)){
        for(blockIndex = 0;blockIndex < nBlocksTested;blockIndex++){
            for(chunkIndex = 0;chunkIndex < chunksToTest;chunkIndex++){
                for(i = 0;i < yfs->spare_size;i++){
                    fprintf(stderr, "%02x", allSpares[blockIndex * yfs->spare_size * chunksToTest + chunkIndex * yfs->spare_size + i]);
                }
                fprintf(stderr, "\n");
            }
        }
    }

    // Test all indices into the spare area (that leave enough space for all 16 bytes)
    for(currentOffset = 0;currentOffset <= yfs->spare_size - 16;currentOffset++){
        goodOffset = 1;
        for(blockIndex = 0;blockIndex < nBlocksTested;blockIndex++){
            for(chunkIndex = 1;chunkIndex < chunksToTest;chunkIndex++){

                lastChunkBase = blockIndex * yfs->spare_size * chunksToTest + (chunkIndex - 1) * yfs->spare_size;
                thisChunkBase = lastChunkBase + yfs->spare_size;

                // Seq num should not be all 0xff (we tested earlier that the chunk has been initialized)
                if((0xff == allSpares[thisChunkBase + currentOffset]) &&
                    (0xff == allSpares[thisChunkBase + currentOffset + 1]) &&
                    (0xff == allSpares[thisChunkBase + currentOffset + 2]) &&
                    (0xff == allSpares[thisChunkBase + currentOffset + 3])){
                        if(tsk_verbose && (! yfs->autoDetect)){
                            tsk_fprintf(stderr,
                                "yaffs_initialize_spare_format: Eliminating offset %d - invalid sequence number 0xffffffff\n", 
                                currentOffset);
                        }
                        goodOffset = 0;
                        break;
                }

                // Seq num should not be zero
                if((0 == allSpares[thisChunkBase + currentOffset]) &&
                    (0 == allSpares[thisChunkBase + currentOffset + 1]) &&
                    (0 == allSpares[thisChunkBase + currentOffset + 2]) &&
                    (0 == allSpares[thisChunkBase + currentOffset + 3])){
                        if(tsk_verbose && (! yfs->autoDetect)){
                            tsk_fprintf(stderr,
                                "yaffs_initialize_spare_format: Eliminating offset %d - invalid sequence number 0\n", 
                                currentOffset);
                        }
                        goodOffset = 0;
                        break;
                }

                // Seq num should match the previous one in the block
                if((allSpares[lastChunkBase + currentOffset] != allSpares[thisChunkBase + currentOffset]) ||
                    (allSpares[lastChunkBase + currentOffset + 1] != allSpares[thisChunkBase + currentOffset + 1]) ||
                    (allSpares[lastChunkBase + currentOffset + 2] != allSpares[thisChunkBase + currentOffset + 2]) ||
                    (allSpares[lastChunkBase + currentOffset + 3] != allSpares[thisChunkBase + currentOffset + 3])){
                        if(tsk_verbose && (! yfs->autoDetect)){
                            tsk_fprintf(stderr,
                                "yaffs_initialize_spare_format: Eliminating offset %d - did not match previous chunk sequence number\n", 
                                currentOffset);
                        }
                        goodOffset = 0;
                        break;
                }

                // Obj id should not be zero
                if((0 == allSpares[thisChunkBase + currentOffset + 4]) &&
                    (0 == allSpares[thisChunkBase + currentOffset + 5]) &&
                    (0 == allSpares[thisChunkBase + currentOffset + 6]) &&
                    (0 == allSpares[thisChunkBase + currentOffset + 7])){
                        if(tsk_verbose && (! yfs->autoDetect)){
                            tsk_fprintf(stderr,
                                "yaffs_initialize_spare_format: Eliminating offset %d - invalid object id 0\n", 
                                currentOffset);
                        }
                        goodOffset = 0;
                        break;
                }

                // All 16 bytes should not be the same
                // (It is theoretically possible that this could be valid, but incredibly unlikely)
                allSameByte = true;
                for(i = 1;i < 16;i++){
                    if(allSpares[thisChunkBase + currentOffset] != allSpares[thisChunkBase + currentOffset + i]){
                        allSameByte = false;
                        break;
                    }
                }
                if(allSameByte){
                    if(tsk_verbose && (! yfs->autoDetect)){
                        tsk_fprintf(stderr,
                            "yaffs_initialize_spare_format: Eliminating offset %d - all repeated bytes\n", 
                            currentOffset);
                    }
                    goodOffset = 0;
                    break;
                }

            } // End of loop over chunks

            if(!goodOffset){ // Break out of loop over blocks
                break;
            }
        }
        if(goodOffset){

            // Note that we've found an offset that is at least promising
            if((! goodOffsetFound) && (! okOffsetFound)){
                bestOffset = currentOffset;
            }
            okOffsetFound = 1;

            if(tsk_verbose && (! yfs->autoDetect)){
                tsk_fprintf(stderr,
                    "yaffs_initialize_spare_format: Found potential spare offsets:  %d (sequence number), %d (object id), %d (chunk id), %d (n bytes)\n",
                    currentOffset, currentOffset+4, currentOffset+8, currentOffset+12);
            }

            // Now do some more tests
            // Really need some more real-world test data to do this right.
            int possibleError = 0;

            // We probably don't want the first byte to always be 0xff
            int firstByteFF = 1;
            for(blockIndex = 0;blockIndex < nBlocksTested;blockIndex++){
                for(chunkIndex = 1;chunkIndex < chunksToTest;chunkIndex++){
                    if(allSpares[blockIndex * yfs->spare_size * chunksToTest + chunkIndex * yfs->spare_size + currentOffset] != 0xff){
                        firstByteFF = 0;
                    }
                }
            }

            if(firstByteFF){
                if(tsk_verbose && (! yfs->autoDetect)){
                    tsk_fprintf(stderr,
                        "yaffs_initialize_spare_format:  Previous data starts with all 0xff bytes. Looking for better offsets.\n");
                }
                possibleError = 1;
            }

            if(! possibleError){

                // If we already have a good offset, print this one out but don't record it
                if(! goodOffsetFound){

                    goodOffsetFound = 1;
                    bestOffset = currentOffset;

                    // Offset passed additional testing and we haven't seen an earlier good one, so go ahead and use it
                    if(tsk_verbose && (! yfs->autoDetect)){
                        tsk_fprintf(stderr,
                            "yaffs_initialize_spare_format:  Previous offsets appear good - will use as final offsets\n");
                    }

                }
                else{
                    // Keep using the old one
                    if(tsk_verbose && (! yfs->autoDetect)){
                        tsk_fprintf(stderr,
                            "yaffs_initialize_spare_format:  Previous offsets appear good but staying with earlier valid ones\n");
                    }
                }
            }
        }
    }

    free(spareBuffer);
    free(allSpares);

    if(okOffsetFound || goodOffsetFound){
        // Record everything
        yfs->spare_seq_offset = bestOffset;
        yfs->spare_obj_id_offset = bestOffset + 4;
        yfs->spare_chunk_id_offset = bestOffset + 8;
        yfs->spare_nbytes_offset = bestOffset + 12;

        if(tsk_verbose && (! yfs->autoDetect)){
            tsk_fprintf(stderr,
                "yaffs_initialize_spare_format: Final offsets: %d (sequence number), %d (object id), %d (chunk id), %d (n bytes)\n",
                bestOffset, bestOffset+4, bestOffset+8, bestOffset+12);
            tsk_fprintf(stderr,
                "If these do not seem valid: %s\n", YAFFS_HELP_MESSAGE);
        }
        return TSK_OK;
    }
    else{
        return TSK_ERR;
    }
}