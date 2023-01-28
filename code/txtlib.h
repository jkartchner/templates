#ifndef __txtlib_h__
#define __txtlib_h__

bool is_digit (char c)
{
    if(c > 47 && c < 58)
    {
        return true;
    }
    return false;
}

bool is_space (char c)
{
    if(c == ' ' || c == '\t' || c == '\r')
    {
        return true;
    }
    return false;
}

// this is mostly the strtol function in glibc
// removed range error checking, most functionality
// other than that used by atoi: base 10 numbers
// TODO: atoi just sends NULL to endptr; why is it here?
//       can I remove it? Should I do alpha and range checking?
static long txt_to_int (const char *nptr, char **endptr, int base)
{
    register int c;
    register const char *s;
    register long acc;//, cutoff;
    register int neg, any;//, cutlim;

    s = nptr;
    // deal with whitespace
    do
    {
        c = (unsigned char)*s++;
    } while(is_space(c));
    if(c == '-')
    {
        neg = 1;
        c = *s++;
    }
    else
    {
        neg = 0;
        if(c == '+')
        {
            c = *s++;
        }
    }
    if((base == 0 || base == 16) && 
            c == '0' &&
            (*s == 'x' || *s == 'X'))
    {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
    {
        base = c == '0' ? 8 : 10;
    }

    // glib here calcs the cutoff limit for range errors
    // which I will save for later TODO: deal with range err
    for(acc = 0, any = 0;; c = (unsigned char) *s++)
    {
        if(is_digit(c))
        {
            c -= 48;
        }
        // not dealing with alpha chars for hex
        // for now TODO: deal with alphanumerics for hex

        //else if(isalpha(c))
        //{
            //c -= kk
        else { break; }
        if(c >= base) { break; }
        if(any < 0) { continue; }
        if(neg)
        {
            any = 1;
            acc *= base;
            acc -= c;
        } 
        else
        {
            any = 1;
            acc *= base;
            acc += c;
        }
        if(endptr != 0) // atoi always sends NULL (0)
        {
            *endptr = (char *) (any ? s - 1 : nptr);
        }

    } 
    return acc;
}

int a_to_i(const char *str)
{
    return ((int)txt_to_int(str, (char **)0, 10));
}



// overloading combine path filename func
void combine_path_filename(char *path, char *file_name, char *out)
{
    short counter = 0;
    while(path[counter] != '\0' || counter > 1024)
    {
        out[counter] = path[counter];
        counter++;
    }
    
#if _WIN32
    out[counter] = '\\';
#else
    out[counter] = '/';
#endif

    counter++;
    short counter_two = 0;
    while(file_name[counter_two] != '\0' && counter_two < 500)
    {
        out[counter] = file_name[counter_two];
        counter++;
        counter_two++;
    }
    out[counter] = '\0';
}

void combine_path_filename(unsigned char *path, unsigned char *file_name, unsigned char *out)
{
    short counter = 0;
    while(path[counter] != '\0' || counter > 1024)
    {
        out[counter] = path[counter];
        counter++;
    }
    
#if _WIN32
    out[counter] = '\\';
#else
    out[counter] = '/';
#endif

    counter++;
    short counter_two = 0;
    while(file_name[counter_two] != '\0' && counter_two < 500)
    {
        out[counter] = file_name[counter_two];
        counter++;
        counter_two++;
    }
    out[counter] = '\0';
}



// overloading txt count to make very easy to use
int txt_count(unsigned char *from_buffer, int max_len)
{
    int counter = 0;
    while(from_buffer[counter] != '\0' && max_len--) counter++;

    return counter;
}

int txt_count (char *from_buffer, int max_len)
{
    int counter = 0;
    while(from_buffer[counter] != '\0' && max_len--) counter++;

    return counter;
}

int txt_count (const char *from_buffer, int max_len)
{
    int counter = 0;
    while(from_buffer[counter] != '\0' && max_len--) counter++;

    return counter;
}



// overloading txt copy to make super easy to use
int txt_cpy(unsigned char *to_buffer, unsigned char *from_buffer, int max_len)
{
    int counter = 0;
    while(from_buffer[counter] != '\0' && counter < max_len)
    {
        to_buffer[counter] = from_buffer[counter];
        counter++;
    }
    to_buffer[counter] = '\0';
    return counter;
}

int txt_cpy(char *to_buffer, unsigned char *from_buffer, int max_len)
{
    int counter = 0;
    while(from_buffer[counter] != '\0' && counter < max_len)
    {
        to_buffer[counter] = from_buffer[counter];
        counter++;
    }
    to_buffer[counter] = '\0';
    return counter;
}

int txt_cpy(unsigned char *to_buffer, char *from_buffer, int max_len)
{
    int counter = 0;
    while(from_buffer[counter] != '\0' && counter < max_len)
    {
        to_buffer[counter] = from_buffer[counter];
        counter++;
    }
    to_buffer[counter] = '\0';
    return counter;
}

int txt_cpy(char *to_buffer, char *from_buffer, int max_len)
{
    int counter = 0;
    while(from_buffer[counter] != '\0' && counter < max_len)
    {
        to_buffer[counter] = from_buffer[counter];
        counter++;
    }
    to_buffer[counter] = '\0';
    return counter;
}

int txt_cpy(char *to_buffer, const char *from_buffer, int max_len)
{
    int counter = 0;
    while(from_buffer[counter] != '\0' && counter < max_len)
    {
        to_buffer[counter] = from_buffer[counter];
        counter++;
    }
    to_buffer[counter] = '\0';
    return counter;
}

int txt_cpy(unsigned char *to_buffer, const char *from_buffer, int max_len)
{
    int counter = 0;
    while(from_buffer[counter] != '\0' && counter < max_len)
    {
        to_buffer[counter] = from_buffer[counter];
        counter++;
    }
    to_buffer[counter] = '\0';
    return counter;
}




// overloading the zero function to make it crazy easy to use
int mem_zero (unsigned char *to_buffer, unsigned char source, int len)
{
	int counter = len;
	while(len--) *to_buffer++ = source;

	return counter - len;
}

int mem_zero (char *to_buffer, char source, int len)
{
	int counter = len;
	while(len--) *to_buffer++ = source;

	return counter - len;
}

int mem_zero (void *to_buffer, char source, int len)
{
	int counter = len;
	char *dest = (char *)to_buffer;
	while(len--) *dest++ = source;

	return counter - len;
}

int mem_zero (void *to_buffer, int source, int len)
{
	int counter = len;
	int *dest = (int *)to_buffer;
	while(len--) *dest++ = source;

	return counter - len;
}

#endif
