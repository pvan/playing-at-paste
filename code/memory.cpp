




struct memory_block
{
    u32 size;

    void *baseAddress;

    u32 usedSpace = 0;


    u32 spaceLeft()
    {
        return size - usedSpace;
    }

    void *allocate(u32 bytes)
    {
        // return false instead? (but then we'd hve to constantly check if this call fails)
        ASSERT(usedSpace + bytes <= size);

        // find current free address before adding the new bytes to usedspace
        void *baseAddressOfWhatWeAdded = (void*)((u8*)baseAddress + usedSpace);

        usedSpace += bytes;

        return baseAddressOfWhatWeAdded;
    }


};



