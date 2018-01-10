

//----------------

// todo: replace with proper rand lib at some point?
// todo: not even using this file at the moment...

u32 r_state = 2347544;

// all u32 possible
u32 randu32()
{
    return r_state = (69069*r_state) + 362437; // internet magic
}


// 0-1
float randf()
{
    float result = (float)randu32() / (float)(0xffffffff);
	return result;
}
