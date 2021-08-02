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

#include <xt_library_loader.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* This library will be provided by the object created by the 
   xt-pkg-loadlib tool. 

   Observe that the name declared here must match the name used with
   the "-e" option to the tool.
*/
extern xtlib_packaged_library fixed_location_overlay;

/* Declare a function pointer of the same type as the _start function
   provided in the overlay. This function can have whatever prototype you
   choose, but for convenience, it most often takes and returns function
   pointers for calling into and out of the loadable library.
*/

typedef void * (*flo_start_ptr_t)(void * printf_ptr);

flo_start_ptr_t flo_start;

/* Declare a function pointer of same type as the function to call
   in the library. We will call into the library using this function
   pointer.
*/

typedef char * (*foo_ptr_t) (void);

foo_ptr_t foo;

int main(int argc, char * argv[])
{
  printf("Loading overlay...");
  flo_start = (flo_start_ptr_t) xtlib_load_overlay(&fixed_location_overlay);
  if (flo_start != NULL) {
    printf("succeeded\n");
    /* We successfully loaded the library. Now call it's entry point so it can
       get a pointer to printf, and it will return a pointer to its foo 
       function.
       
       If we wanted to use more functions, we could pass and return arrays of
       pointers.
       
       Also, if the interface to the library is just a single function, then
       the entry point itself could serve as that function.       
    */
    char * string_returned_from_foo;
       
    foo = (foo_ptr_t) flo_start(printf);
    string_returned_from_foo = foo();
    printf("function returned the string \"%s\"\n", string_returned_from_foo);
  }
  else {
    printf("failed with error code: %d\n", xtlib_error());
  }
  return 0;
}
