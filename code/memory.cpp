


// very basic chunk of memory that can't deallocate anything (except maybe everything at once)

struct memory_block
{
    u32 size;

    void *baseAddress;

    u32 usedSpace = 0;


    u32 spaceLeft()
    {
        return size - usedSpace;
    }

    void *allocate(u32 bytes)  // "allocate from" really
    {
        // return false instead? (but then we'd hve to constantly check if this call fails)
        ASSERT(usedSpace + bytes <= size);

        // find current free address before adding the new bytes to usedspace
        void *baseAddressOfWhatWeAdded = (void*)((u8*)baseAddress + usedSpace);

        usedSpace += bytes;

        return baseAddressOfWhatWeAdded;
    }


    void create(int size_bytes, void *starting_address)
    {
        size = size_bytes;
        baseAddress = VirtualAlloc(starting_address, size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    }


};



