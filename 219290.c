yaffs_validate_integer_field(std::string numStr){
    unsigned int i;

    // Test if empty
    if(numStr.length() == 0){
        return 1;
    }

    // Test each character
    for(i = 0;i < numStr.length();i++){
        if(isdigit(numStr[i]) == 0){
            return 1;
        }
    }

    return 0;
}