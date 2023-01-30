#ifndef __txtlib_h__
#define __txtlib_h__

#include <stdarg.h>

bool is_digit (char c);
bool is_space (char c);
int a_to_i (const char *str);
char *l_to_txt(long i, char b[], int base_number);
char *i_to_a (int i, char b[], int base_number);

bool txt_is_equal (char *to_buffer, char *from_buffer, int max_len);
bool txt_is_equal (unsigned char *to_buffer, char *from_buffer, int max_len);
bool txt_is_equal (unsigned char *to_buffer, unsigned char *from_buffer, int max_len);
bool txt_is_equal (char *to_buffer, unsigned char *from_buffer, int max_len);
int txt_find(char *parent_string, char *string_to_find, int max_len);
int txt_find(unsigned char *parent_string, char *string_to_find, int max_len);
int txt_find(unsigned char *parent_string, unsigned char *string_to_find, int max_len);
int txt_find(char *parent_string, unsigned char *string_to_find, int max_len);
void combine_path_filename(char *path, char *file_name, char *out);
void combine_path_filename(unsigned char *path, unsigned char *file_name, unsigned char *out);
int txt_count(unsigned char *from_buffer, int max_len);
int txt_count (char *from_buffer, int max_len);
int txt_count (const char *from_buffer, int max_len);
int txt_cpy(unsigned char *to_buffer, unsigned char *from_buffer, int max_len);
int txt_cpy(char *to_buffer, unsigned char *from_buffer, int max_len);
int txt_cpy(unsigned char *to_buffer, char *from_buffer, int max_len);
int txt_cpy(char *to_buffer, char *from_buffer, int max_len);
int txt_cpy(char *to_buffer, const char *from_buffer, int max_len);
int mem_zero (unsigned char *to_buffer, unsigned char source, int len);
int mem_zero (char *to_buffer, char source, int len);
int mem_zero (void *to_buffer, char source, int len);
int mem_zero (void *to_buffer, int source, int len);

int printflist_format(int buff_size, char *to_buffer, const char *from_buffer, va_list args);
int printf_format(int buff_size, char *to_buffer, const char *from_buffer, ...);



// INTEGER OPERATIONS WITH TEXT



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

int a_to_i (const char *str)
{
    return ((int)txt_to_int(str, (char **)0, 10));
}


// stack overflow: better than I would have coded it
// though I changed it to accommodate base 2, 16
char *l_to_txt(long i, char b[], int base_number)
{
    char const decdigit[] = "0123456789";
    char const hexdigit[] = "0123456789abcdef";
    char const bindigit[] = "01";
    const char *digit;
    switch(base_number)
    {
        case 10:
            digit = decdigit;
            break;
        case 16:
            digit = hexdigit;
            break;
        case 2:
            digit = bindigit;
            break;
        default:
            digit = decdigit;
    }
    char *p = b;
    if(i < 0 && base_number != 16)
    {
        *p++ = '-';
        i *= -1; // flip to positive
    }
    if(base_number == 16)
    {
        *p++ = '0';
        *p++ = 'x';
    }
    int shifter = i;
    do {    // move to end of int
        ++p;
        shifter = shifter/base_number;
    } while(shifter);

    *p = '\0';

    do {    // move back, inserting as you go
        *--p = digit[i % base_number];
        i = i / base_number;
    } while(i);

    return b;
}

char *i_to_a (int i, char b[], int base_number)
{
    char const decdigit[] = "0123456789";
    char const hexdigit[] = "0123456789abcdef";
    char const bindigit[] = "01";
    const char *digit;
    switch(base_number)
    {
        case 10:
            digit = decdigit;
            break;
        case 16:
            digit = hexdigit;
            break;
        case 2:
            digit = bindigit;
            break;
        default:
            digit = decdigit;
    }
    char *p = b;
    if(i < 0 && base_number != 16)
    {
        *p++ = '-';
        i *= -1; // flip to positive
    }
    if(base_number == 16)
    {
        *p++ = '0';
        *p++ = 'x';
    }
    int shifter = i;
    do {    // move to end of int
        ++p;
        shifter = shifter/base_number;
    } while(shifter);

    *p = '\0';

    do {    // move back, inserting as you go
        *--p = digit[i % base_number];
        i = i / base_number;
    } while(i);

    return b;
}




// TEXT COPY AND MOVEMENT OPERATIONS


// overloading for ease of use
// this is the generic string compare func
bool txt_is_equal (char *to_buffer, char *from_buffer, int max_len)
{
    int counter = 0;
    while(from_buffer[counter] != '\0' && max_len--)
    {
        if(to_buffer[counter] != from_buffer[counter]) 
	{
            return false;
        }            
        counter++;
    }
    return true;
}

// these are the overloads
bool txt_is_equal (unsigned char *to_buffer, char *from_buffer, int max_len)
{
	char *tmp = (char *)to_buffer;
	return txt_is_equal(tmp, from_buffer, max_len);
}

bool txt_is_equal (unsigned char *to_buffer, unsigned char *from_buffer, int max_len)
{
	char *tmp = (char *)to_buffer;
	char *tmp2 = (char *)from_buffer;
	return txt_is_equal(tmp, tmp2, max_len);
}

bool txt_is_equal (char *to_buffer, unsigned char *from_buffer, int max_len)
{
	char *tmp = (char *)from_buffer;
	return txt_is_equal(to_buffer, tmp, max_len);
}

// overloading for ease of use
// standard naive find in string function
// inadvisable to use with strings over 2k due to speed
// TODO: improve with binary search or other algo
int txt_find(char *parent_string, char *string_to_find, int max_len)
{
    int counter = 0;
    while(parent_string[counter] != '\0' && max_len--)
    {
        if(parent_string[counter] == string_to_find[0]) 
	{
            int side_counter = 1;
            int string_to_find_counter = 1;
            bool is_not_match = false;
            while(string_to_find[string_to_find_counter] != '\0')
            {
                if(parent_string[counter + side_counter] != 
                   string_to_find[string_to_find_counter])
		{
                    is_not_match = true;
                    break;
                }

                string_to_find_counter++;
                side_counter++;
            }

            if(!is_not_match) 
                return counter;
        }            
        counter++;
    }
    return -1;
}

int txt_find(unsigned char *parent_string, char *string_to_find, int max_len)
{
	char * tmp = (char *)parent_string;
	return txt_find(tmp, string_to_find, max_len);
}

int txt_find(unsigned char *parent_string, unsigned char *string_to_find, int max_len)
{
	char * tmp = (char *)parent_string;
	char * tmp2 = (char *)string_to_find;
	return txt_find(tmp, tmp2, max_len);
}

int txt_find(char *parent_string, unsigned char *string_to_find, int max_len)
{
	char * tmp = (char *)string_to_find;
	return txt_find(parent_string, tmp, max_len);
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
// TODO: make generic function with all the copy pasta and 
// 	 have overloaded funcs call that
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
// TODO: make generic function with all the copy pasta and 
// 	 have overloaded funcs call that
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



// PRINTF FORMATTING FUNCTIONS


int printflist_format(int buff_size, 
                    char *to_buffer, 
                    const char *from_buffer, 
                    va_list args)
{

    int counter = 0;
    int bytes_written = 0;
    int max_isize = 50;
    char b[50]; // for int conversion
    if(buff_size <= 0) { return 0; }
    while(from_buffer[counter] != '\0' && 
           bytes_written < buff_size - 1)
    {
        if(from_buffer[counter] == '%')
        {
            switch(from_buffer[counter + 1])
            {
                case 'd':
                    bytes_written += txt_cpy(
                        (unsigned char *)to_buffer + bytes_written, 
                        (unsigned char *)i_to_a (
						 va_arg(args, int), 
						 b, 10),
			max_isize);
                    counter++;
                    break;
                case 's':
                    bytes_written += txt_cpy(
                        (unsigned char *)to_buffer + bytes_written,
                        (unsigned char *)va_arg(args, char *),
			buff_size - bytes_written);
                    counter++;
                    break;
                case 'p':
                    //bytes_written += txt_copy( // WARNING: converts lower 1/2 of addy
                     //   (unsigned char *)to_buffer + bytes_written, 
                      //  (unsigned char *)l_to_txt(
                       //                           (long)va_arg(args, int), 
                        //                          b, 16),
			//max_isize);
                    counter++;
                    break;
                default:
                    break;
            }
            counter++;
        }
        else 
        {
            to_buffer[bytes_written] = from_buffer[counter];
            counter++;
            bytes_written++;
        }
    }
    to_buffer[bytes_written + 1] = '\0';
    to_buffer[buff_size - 1] = '\0'; // lazy validate string
    return bytes_written;
}

int printf_format(int buff_size, char *to_buffer, const char *from_buffer, ...)
{
    va_list args;

    va_start(args, from_buffer);
    int result = printflist_format(buff_size, to_buffer, from_buffer, args);
    va_end(args);

    return result;
}


#endif
