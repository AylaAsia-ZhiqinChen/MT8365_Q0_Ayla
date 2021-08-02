/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2012-2013 by Tensilica Inc. ALL RIGHTS RESERVED.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "xt_library_loader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* This library will be provided by the object created by the 
   package_loadable_lib.pl tool. 

   Observe that the name declared here must match the name used with
   the "-e" option to the tool.
*/
extern xtlib_packaged_library pi_library_sl;

/* Declare a function pointer of the same type as the _start function
   provided in the overlay. This function can have whatever prototype you
   choose, but for convenience, it most often takes and returns function
   pointers for calling into and out of the loadable library.
*/

typedef void * (*pilsl_start_ptr_t)(void * printf_ptr);

pilsl_start_ptr_t pilsl_start;

/* Declare a function pointer of same type as the function to call
   in the library. We will call into the library using this function
   pointer.
*/

typedef char * (*foo_ptr_t) (void);

foo_ptr_t foo_ptr;

int main(int argc, char * argv[])
{
  unsigned int code_bytes, data_bytes;
  if (xtlib_split_pi_library_size (&pi_library_sl, &code_bytes, &data_bytes) != XTLIB_NO_ERR) {
    printf ("xtlib_split_pi_library_size failed with error: %d\n", xtlib_error ());
    return 0;
  }

  void * data_memory = malloc (data_bytes);
  void * code_memory = malloc (code_bytes);

  if (!code_memory || !data_memory) {
    printf ("malloc failed\n");
    return 0;
  }

  xtlib_pil_info lib_info;

  printf("Loading position independent library %d-byte code at 0x%x and %d-byte data at 0x%x...", 
	 code_bytes, (unsigned int)code_memory, data_bytes, (unsigned int)data_memory);

  fflush(NULL);
  pilsl_start = (pilsl_start_ptr_t) xtlib_load_split_pi_library(&pi_library_sl, code_memory, data_memory, &lib_info);
  if (pilsl_start != NULL) {
    printf(" succeeded\n");
    /* We successfully loaded the library. Now call it's entry point so it can
       get a pointer to printf, and it will return a pointer to its foo 
       function.
       
       If we wanted to use more functions, we could pass and return arrays of
       pointers.
       
       Also, if the interface to the library is just a single function, then
       the entry point itself could serve as that function.       
    */
    char * string_returned_from_foo;
       
    foo_ptr = (foo_ptr_t) pilsl_start(printf);
    string_returned_from_foo = foo_ptr();
    printf(" function returned the string \"%s\"\n", string_returned_from_foo);

    /* unloading library */
    printf ("unloading library\n");

    xtlib_unload_pi_library (&lib_info);
  }
  else {
    printf(" failed with error code: %d\n", xtlib_error());
  }

  free (data_memory);
  free (code_memory);

  return 0;
}
