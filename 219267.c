yaffs_validate_config_file(std::map<std::string, std::string> & paramMap){
    int offset_field_count;

    // Make a list of all fields to test
    std::set<std::string> integerParams;
    integerParams.insert(YAFFS_CONFIG_SEQ_NUM_STR);
    integerParams.insert(YAFFS_CONFIG_OBJ_ID_STR);
    integerParams.insert(YAFFS_CONFIG_CHUNK_ID_STR);
    integerParams.insert(YAFFS_CONFIG_PAGE_SIZE_STR);
    integerParams.insert(YAFFS_CONFIG_SPARE_SIZE_STR);
    integerParams.insert(YAFFS_CONFIG_CHUNKS_PER_BLOCK_STR);

    // If the parameter is set, verify that the value is an int
    for(std::set<std::string>::iterator it = integerParams.begin();it != integerParams.end();it++){
        if((paramMap.find(*it) != paramMap.end()) && 
            (0 != yaffs_validate_integer_field(paramMap[*it]))){
            tsk_error_reset();
            tsk_error_set_errno(TSK_ERR_FS);
            tsk_error_set_errstr(
                        "yaffs_validate_config_file: Empty or non-integer value for Yaffs2 parameter \"%s\". %s", (*it).c_str(), YAFFS_HELP_MESSAGE);
            return 1;
        }
    }

    // Check that we have all three spare offset fields, or none of the three
    offset_field_count = 0;
    if(paramMap.find(YAFFS_CONFIG_SEQ_NUM_STR) != paramMap.end()){
        offset_field_count++;
    }
    if(paramMap.find(YAFFS_CONFIG_OBJ_ID_STR) != paramMap.end()){
        offset_field_count++;
    }
    if(paramMap.find(YAFFS_CONFIG_CHUNK_ID_STR) != paramMap.end()){
        offset_field_count++;
    }

    if(! ((offset_field_count == 0) || (offset_field_count == 3))){
            tsk_error_reset();
            tsk_error_set_errno(TSK_ERR_FS);
            tsk_error_set_errstr(
                        "yaffs_validate_config_file: Require either all three spare offset fields or none. %s", YAFFS_HELP_MESSAGE);
            return 1;
    }

    // Make sure there aren't any unexpected fields present
    for(std::map<std::string, std::string>::iterator it = paramMap.begin(); it != paramMap.end();it++){
        if(integerParams.find(it->first) == integerParams.end()){
            tsk_error_reset();
            tsk_error_set_errno(TSK_ERR_FS);
            tsk_error_set_errstr(
                        "yaffs_validate_config_file: Found unexpected field in config file (\"%s\"). %s", it->first.c_str(), YAFFS_HELP_MESSAGE);
            return 1;
        }
    }

    return 0;
}