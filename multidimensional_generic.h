/*--------------------------------------- MDG: Multidimensional Generic v1.0.0 -----------------------------------------

Introduction:

  This tiny library provides macros that are useful, in particular, for accomodating multidimensional arrays (up to six
  dimensions) in _Generic expressions. It requires C23, or C11 and compiler support for typeof and typeof_unqual (which
  are available in all major compilers).

API:

  MDG_IS_ARRAY(x)

    This macro evaluates to 1 if x is an array.

  MDG_TYPEOF_BASE(x)

    If x is an array with up to six dimensions, this macro evaluates to x's base type.
    If x is not an array, this macro evaluates to x's type.
    If x is an array with more than six dimensions, this macro generates a compiler error.
    This macro is mainly useful as part of the controlling expressions of a _Generic expression.

  MDG_MDG_DIM_COUNT(x)

    If x is an array with up to six dimensions, this macro evaluates to the number of dimensions.
    If x is not an array, this macro evaluates to 0.
    If x is an array with more than six dimensions, this macro generates a compiler error.

  MDG_DIM1_EL_SIZE(x)
  MDG_DIM2_EL_SIZE(x)
  MDG_DIM3_EL_SIZE(x)
  MDG_DIM4_EL_SIZE(x)
  MDG_DIM5_EL_SIZE(x)
  MDG_DIM6_EL_SIZE(x)

    If x is an array with up to six dimensions, these macros evaluate to the size of one element of the corresponing
    dimension.
    Otherwise, it evaluates to a junk value that should be ignored.

  MDG_DIM1_EL_COUNT(x)
  MDG_DIM2_EL_COUNT(x)
  MDG_DIM3_EL_COUNT(x)
  MDG_DIM4_EL_COUNT(x)
  MDG_DIM5_EL_COUNT(x)
  MDG_DIM6_EL_COUNT(x)

    If x is an array that has the specified dimension, these macros evaluate to the number of elements in that
    dimension.
    Otherwise, it evaluates to a junk value that should be ignored.

  MDG_CTRL_EXPR(x)

    This macro, when passed an ordinary variable or an array with up to six dimensions, can be used as the controlling
    expression of a _Generic expression to distinguish between array types and other types (see example 2 below).
    You probably do not need this macro (see examples 1 and 3 below).

  MDG_ASSOC_RAW(t)
  MDG_ASSOC_1D(t)
  MDG_ASSOC_2D(t)
  MDG_ASSOC_3D(t)
  MDG_ASSOC_4D(t)
  MDG_ASSOC_5D(t)
  MDG_ASSOC_6D(t)

    These macros should be used to specify the association types in a _Generic expression that uses MDG_CTRL_EXPR to
    specify the controlling expression (see example 2 below).

Examples:

  1. Using MDG_TYPEOF_BASE to match against the base type:

    #include <stdio.h>
    #include <stdlib.h>
    #include "multidimensional_generic.h"

    static void print_ints(const void *data, size_t total_size)
    {
      const int *i = data;
      while(total_size > 0)
      {
        printf("%d ", *i++);
        total_size -= sizeof(int);
      }
    }

    #define PRINT(x) _Generic((MDG_TYPEOF_BASE(*(x))){0}, \
      int: print_ints                                     \
    )((x), sizeof(*(x)))                                  \

    int main(void)
    {
      int raw;
      int array_1d[1];
      int array_2d[1][2];
      int array_3d[1][2][3];
      int array_4d[1][2][3][4];
      int array_5d[1][2][3][4][5];
      int array_6d[1][2][3][4][5][6];

      for( int i = 0; i < sizeof(array_6d) / sizeof(int); ++i)
      {
        if(i < sizeof( raw ) / sizeof(int)) *((int *)&raw + i) = i;
        if(i < sizeof( array_2d ) / sizeof(int)) *((int *)array_2d + i) = i;
        if(i < sizeof( array_3d ) / sizeof(int)) *((int *)array_3d + i) = i;
        if(i < sizeof( array_4d ) / sizeof(int)) *((int *)array_4d + i) = i;
        if(i < sizeof( array_5d ) / sizeof(int)) *((int *)array_5d + i) = i;
        if(i < sizeof( array_6d ) / sizeof(int)) *((int *)array_6d + i) = i;
      }

      PRINT(&raw);
      printf("\n");
      PRINT(&array_1d);
      printf("\n");
      PRINT(&array_2d);
      printf("\n");
      PRINT(&array_3d);
      printf("\n");
      PRINT(&array_4d);
      printf("\n");
      PRINT(&array_5d);
      printf("\n");
      PRINT(&array_6d);
      printf("\n");
    }

  2. Using MDG_CTRL_EXPR and MDG_ASSOC_*D to match specifically against the number of array dimensions:

    #include <stdio.h>
    #include "multidimensional_generic.h"

    #define PRINT_TYPEOF(x) printf(           \
      _Generic(MDG_CTRL_EXPR(x),              \
        MDG_ASSOC_RAW(int): "int",            \
        MDG_ASSOC_1D(int):  "int[]",          \
        MDG_ASSOC_2D(int):  "int[][]",        \
        MDG_ASSOC_3D(int):  "int[][][]",      \
        MDG_ASSOC_4D(int):  "int[][][][]",    \
        MDG_ASSOC_5D(int):  "int[][][][][]",  \
        MDG_ASSOC_6D(int):  "int[][][][][][]" \
      )                                       \
    )                                         \

    int main(void)
    {
      int raw;
      int array_1d[1];
      int array_2d[1][2];
      int array_3d[1][2][3];
      int array_4d[1][2][3][4];
      int array_5d[1][2][3][4][5];
      int array_6d[1][2][3][4][5][6];

      PRINT_TYPEOF(raw);
      printf("\n");
      PRINT_TYPEOF(array_1d);
      printf("\n");
      PRINT_TYPEOF(array_2d);
      printf("\n");
      PRINT_TYPEOF(array_3d);
      printf("\n");
      PRINT_TYPEOF(array_4d);
      printf("\n");
      PRINT_TYPEOF(array_5d);
      printf("\n");
      PRINT_TYPEOF(array_6d);
      printf("\n");
    }

  3. Using MDG_DIM?_EL_SIZE to provide array dimension data to the called function:

    #include <stdio.h>
    #include <stdlib.h>
    #include "multidimensional_generic.h"

    static void print_int(const void *data)
    {
      printf("%d ", *(const int *)data);
    }

    static void print_multiple(
      const void *data,
      int dim_count,
      const size_t *dim_el_sizes,
      void (*print_func)(const void *)
    )
    {
      if(dim_count == 0)
        print_func(data);
      else
      {
        printf("[ ");

        for(const char *i = data, *end = i + dim_el_sizes[0]; i < end; i += dim_el_sizes[1])
          print_multiple(i, dim_count - 1, dim_el_sizes + 1, print_func);

        printf( "] ");
      }
    }

    #define PRINT(x) print_multiple(       \
      (x),                                 \
      MDG_MDG_DIM_COUNT(*(x)),             \
      (size_t[]){                          \
        sizeof(*(x)),                      \
        MDG_DIM1_EL_SIZE(*(x)),            \
        MDG_DIM2_EL_SIZE(*(x)),            \
        MDG_DIM3_EL_SIZE(*(x)),            \
        MDG_DIM4_EL_SIZE(*(x)),            \
        MDG_DIM5_EL_SIZE(*(x)),            \
        MDG_DIM6_EL_SIZE(*(x))             \
      },                                   \
      _Generic((MDG_TYPEOF_BASE(*(x))){0}, \
        int: print_int                     \
      )                                    \
    )                                      \

    int main(void)
    {
      int raw;
      int array_1d[1];
      int array_2d[1][2];
      int array_3d[1][2][3];
      int array_4d[1][2][3][4];
      int array_5d[1][2][3][4][5];
      int array_6d[1][2][3][4][5][6];

      for( int i = 0; i < sizeof(array_6d) / sizeof(int); ++i)
      {
        if(i < sizeof( raw ) / sizeof(int)) *((int *)&raw + i) = i;
        if(i < sizeof( array_2d ) / sizeof(int)) *((int *)array_2d + i) = i;
        if(i < sizeof( array_3d ) / sizeof(int)) *((int *)array_3d + i) = i;
        if(i < sizeof( array_4d ) / sizeof(int)) *((int *)array_4d + i) = i;
        if(i < sizeof( array_5d ) / sizeof(int)) *((int *)array_5d + i) = i;
        if(i < sizeof( array_6d ) / sizeof(int)) *((int *)array_6d + i) = i;
      }

      PRINT(&raw);
      printf("\n");
      PRINT(&array_1d);
      printf("\n");
      PRINT(&array_2d);
      printf("\n");
      PRINT(&array_3d);
      printf("\n");
      PRINT(&array_4d);
      printf("\n");
      PRINT(&array_5d);
      printf("\n");
      PRINT(&array_6d);
      printf("\n");
    }

Version history:

  --/05/2026 1.0.0: Initial release.

License (MIT):

  Copyright (c) 2026 Jackson L. Allan

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
  documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
  persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
  Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#ifndef MULTIMDG_DIMENSIONAL_GENERIC_H
#define MULTIMDG_DIMENSIONAL_GENERIC_H

#if __STDC_VERSION__ >= 202311L // C23.
#define MDG_TYPEOF        typeof
#define MDG_TYPEOF_UNQUAL typeof_unqual
#else
#define MDG_TYPEOF        __typeof__
#define MDG_TYPEOF_UNQUAL __typeof_unqual__
#endif

// Disable Clang warning triggered when an array is passed into the MDG_IS_ARRAY macro.
// Although this warning could instead be temporarily disabled within the macro, the macro is invoked frequently in
// other macros, so disabling it globally may lead to better compile speed.
#ifdef __clang__
#pragma clang diagnostic ignored "-Wunreachable-code-generic-assoc"
#endif

#define MDG_IS_ARRAY(x) _Generic((x), MDG_TYPEOF_UNQUAL(x): 0, default: 1)

// Integral-constant type, similar to C++'s std::integral_constant.
#define MDG_IC_TYPE(i) MDG_TYPEOF(char (*)[1 + (i)])

#define MDG_MDG_DIM_COUNT(x)                                                                                         \
_Generic(                                                                                                            \
  /* Controlling expression */                                                                                       \
  (void (*)(                                                                                                         \
    MDG_IC_TYPE(MDG_IS_ARRAY(x)) has_dim1,                                                                           \
    MDG_IC_TYPE(MDG_IS_ARRAY(    *_Generic(has_dim1, MDG_IC_TYPE(0):     (char *)0, MDG_IC_TYPE(1): (x)))) has_dim2, \
    MDG_IC_TYPE(MDG_IS_ARRAY(   **_Generic(has_dim2, MDG_IC_TYPE(0):    (char **)0, MDG_IC_TYPE(1): (x)))) has_dim3, \
    MDG_IC_TYPE(MDG_IS_ARRAY(  ***_Generic(has_dim3, MDG_IC_TYPE(0):   (char ***)0, MDG_IC_TYPE(1): (x)))) has_dim4, \
    MDG_IC_TYPE(MDG_IS_ARRAY( ****_Generic(has_dim4, MDG_IC_TYPE(0):  (char ****)0, MDG_IC_TYPE(1): (x)))) has_dim5, \
    MDG_IC_TYPE(MDG_IS_ARRAY(*****_Generic(has_dim5, MDG_IC_TYPE(0): (char *****)0, MDG_IC_TYPE(1): (x)))) has_dim6  \
  ))0,                                                                                                               \
  /* Association list */                                                                                             \
  void (*)(MDG_IC_TYPE(0), MDG_IC_TYPE(0), MDG_IC_TYPE(0), MDG_IC_TYPE(0), MDG_IC_TYPE(0), MDG_IC_TYPE(0)): 0,       \
  void (*)(MDG_IC_TYPE(1), MDG_IC_TYPE(0), MDG_IC_TYPE(0), MDG_IC_TYPE(0), MDG_IC_TYPE(0), MDG_IC_TYPE(0)): 1,       \
  void (*)(MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(0), MDG_IC_TYPE(0), MDG_IC_TYPE(0), MDG_IC_TYPE(0)): 2,       \
  void (*)(MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(0), MDG_IC_TYPE(0), MDG_IC_TYPE(0)): 3,       \
  void (*)(MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(0), MDG_IC_TYPE(0)): 4,       \
  void (*)(MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(0)): 5,       \
  void (*)(MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1)): 6        \
)                                                                                                                    \

#define MDG_TYPEOF_BASE(x)                     \
MDG_TYPEOF(                                    \
  ******_Generic(                              \
    /* Controlling expression */               \
    (MDG_IC_TYPE(MDG_MDG_DIM_COUNT(x)))0,      \
    /* Association list */                     \
    MDG_IC_TYPE(0): (MDG_TYPEOF( x ) ******)0, \
    MDG_IC_TYPE(1): (MDG_TYPEOF( x ) *****)0,  \
    MDG_IC_TYPE(2): (MDG_TYPEOF( x ) ****)0,   \
    MDG_IC_TYPE(3): (MDG_TYPEOF( x ) ***)0,    \
    MDG_IC_TYPE(4): (MDG_TYPEOF( x ) **)0,     \
    MDG_IC_TYPE(5): (MDG_TYPEOF( x ) *)0,      \
    MDG_IC_TYPE(6): (MDG_TYPEOF(x)){0}         \
  )                                            \
)                                              \

#define MDG_DIM1_EL_SIZE(x) sizeof(*_Generic( (x), MDG_TYPEOF_UNQUAL(x): (char *)0, default: (x)))

#define MDG_DIM2_EL_SIZE(x)                                                                                    \
sizeof(                                                                                                        \
  **_Generic(                                                                                                  \
    /* Controlling expression */                                                                               \
    (void (*)(                                                                                                 \
      MDG_IC_TYPE(MDG_IS_ARRAY(x)) has_dim1,                                                                   \
      MDG_IC_TYPE(MDG_IS_ARRAY(*_Generic(has_dim1, MDG_IC_TYPE(0): (char *)0, MDG_IC_TYPE(1): (x)))) has_dim2  \
    ))0,                                                                                                       \
    /* Association list */                                                                                     \
    void (*)(MDG_IC_TYPE(1), MDG_IC_TYPE(1)): (x),                                                             \
    default: (char **)0                                                                                        \
  )                                                                                                            \
)                                                                                                              \

#define MDG_DIM3_EL_SIZE(x)                                                                                      \
sizeof(                                                                                                          \
  ***_Generic(                                                                                                   \
    /* Controlling expression */                                                                                 \
    (void (*)(                                                                                                   \
      MDG_IC_TYPE(MDG_IS_ARRAY(x)) has_dim1,                                                                     \
      MDG_IC_TYPE(MDG_IS_ARRAY( *_Generic(has_dim1, MDG_IC_TYPE(0):  (char *)0, MDG_IC_TYPE(1): (x)))) has_dim2, \
      MDG_IC_TYPE(MDG_IS_ARRAY(**_Generic(has_dim2, MDG_IC_TYPE(0): (char **)0, MDG_IC_TYPE(1): (x)))) has_dim3  \
    ))0,                                                                                                         \
    /* Association list */                                                                                       \
    void (*)(MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1)): (x),                                               \
    default: (char ***)0                                                                                         \
  )                                                                                                              \
)                                                                                                                \

#define MDG_DIM4_EL_SIZE(x)                                                                                        \
sizeof(                                                                                                            \
  ****_Generic(                                                                                                    \
    /* Controlling expression */                                                                                   \
    (void (*)(                                                                                                     \
      MDG_IC_TYPE(MDG_IS_ARRAY(x)) has_dim1,                                                                       \
      MDG_IC_TYPE(MDG_IS_ARRAY(  *_Generic(has_dim1, MDG_IC_TYPE(0):   (char *)0, MDG_IC_TYPE(1): (x)))) has_dim2, \
      MDG_IC_TYPE(MDG_IS_ARRAY( **_Generic(has_dim2, MDG_IC_TYPE(0):  (char **)0, MDG_IC_TYPE(1): (x)))) has_dim3, \
      MDG_IC_TYPE(MDG_IS_ARRAY(***_Generic(has_dim3, MDG_IC_TYPE(0): (char ***)0, MDG_IC_TYPE(1): (x)))) has_dim4  \
    ))0,                                                                                                           \
    /* Association list */                                                                                         \
    void (*)(MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1)): (x),                                 \
    default: (char ****)0                                                                                          \
  )                                                                                                                \
)                                                                                                                  \

#define MDG_DIM5_EL_SIZE(x)                                                                                          \
sizeof(                                                                                                              \
  *****_Generic(                                                                                                     \
    /* Controlling expression */                                                                                     \
    (void (*)(                                                                                                       \
      MDG_IC_TYPE(MDG_IS_ARRAY(x)) has_dim1,                                                                         \
      MDG_IC_TYPE(MDG_IS_ARRAY(   *_Generic(has_dim1, MDG_IC_TYPE(0):    (char *)0, MDG_IC_TYPE(1): (x)))) has_dim2, \
      MDG_IC_TYPE(MDG_IS_ARRAY(  **_Generic(has_dim2, MDG_IC_TYPE(0):   (char **)0, MDG_IC_TYPE(1): (x)))) has_dim3, \
      MDG_IC_TYPE(MDG_IS_ARRAY( ***_Generic(has_dim3, MDG_IC_TYPE(0):  (char ***)0, MDG_IC_TYPE(1): (x)))) has_dim4, \
      MDG_IC_TYPE(MDG_IS_ARRAY(****_Generic(has_dim4, MDG_IC_TYPE(0): (char ****)0, MDG_IC_TYPE(1): (x)))) has_dim5  \
    ))0,                                                                                                             \
    /* Association list */                                                                                           \
    void (*)(MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1)): (x),                   \
    default: (char *****)0                                                                                           \
  )                                                                                                                  \
)                                                                                                                    \

#define MDG_DIM6_EL_SIZE(x)                                                                                            \
sizeof(                                                                                                                \
  ******_Generic(                                                                                                      \
    /* Controlling expression */                                                                                       \
    (void (*)(                                                                                                         \
      MDG_IC_TYPE(MDG_IS_ARRAY(x)) has_dim1,                                                                           \
      MDG_IC_TYPE(MDG_IS_ARRAY(    *_Generic(has_dim1, MDG_IC_TYPE(0):    (char  *)0, MDG_IC_TYPE(1): (x)))) has_dim2, \
      MDG_IC_TYPE(MDG_IS_ARRAY(   **_Generic(has_dim2, MDG_IC_TYPE(0):   (char  **)0, MDG_IC_TYPE(1): (x)))) has_dim3, \
      MDG_IC_TYPE(MDG_IS_ARRAY(  ***_Generic(has_dim3, MDG_IC_TYPE(0):  (char  ***)0, MDG_IC_TYPE(1): (x)))) has_dim4, \
      MDG_IC_TYPE(MDG_IS_ARRAY( ****_Generic(has_dim4, MDG_IC_TYPE(0): (char  ****)0, MDG_IC_TYPE(1): (x)))) has_dim5, \
      MDG_IC_TYPE(MDG_IS_ARRAY(*****_Generic(has_dim5, MDG_IC_TYPE(0): (char *****)0, MDG_IC_TYPE(1): (x)))) has_dim6  \
    ))0,                                                                                                               \
    /* Association list */                                                                                             \
    void (*)(MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1), MDG_IC_TYPE(1)): (x),     \
    default: (char ******)0                                                                                            \
  )                                                                                                                    \
)                                                                                                                      \

#define MDG_DIM1_EL_COUNT(x)           (sizeof(x) / MDG_DIM1_EL_SIZE(x))
#define MDG_DIM2_EL_COUNT(x) (MDG_DIM1_EL_SIZE(x) / MDG_DIM2_EL_SIZE(x))
#define MDG_DIM3_EL_COUNT(x) (MDG_DIM2_EL_SIZE(x) / MDG_DIM3_EL_SIZE(x))
#define MDG_DIM4_EL_COUNT(x) (MDG_DIM3_EL_SIZE(x) / MDG_DIM4_EL_SIZE(x))
#define MDG_DIM5_EL_COUNT(x) (MDG_DIM4_EL_SIZE(x) / MDG_DIM5_EL_SIZE(x))
#define MDG_DIM6_EL_COUNT(x) (MDG_DIM5_EL_SIZE(x) / MDG_DIM6_EL_SIZE(x))

#if __STDC_VERSION__ >= 202311L || !defined(__STDC_NO_VLA__)
#define MDG_CTRL_EXPR(x) (void (*)(MDG_TYPEOF(x) *))0
#define MDG_ASSOC_RAW(t) void (*)(MDG_TYPEOF(t) *)
#define MDG_ASSOC_1D(t)  void (*)(MDG_TYPEOF(t)(*)[*])
#define MDG_ASSOC_2D(t)  void (*)(MDG_TYPEOF(t)(*)[*][*])
#define MDG_ASSOC_3D(t)  void (*)(MDG_TYPEOF(t)(*)[*][*][*])
#define MDG_ASSOC_4D(t)  void (*)(MDG_TYPEOF(t)(*)[*][*][*][*])
#define MDG_ASSOC_5D(t)  void (*)(MDG_TYPEOF(t)(*)[*][*][*][*][*])
#define MDG_ASSOC_6D(t)  void (*)(MDG_TYPEOF(t)(*)[*][*][*][*][*][*])
#else // MSVC < C23 fallback.
#define MDG_CTRL_EXPR(x) (void (*)(MDG_TYPEOF_BASE(x), MDG_IC_TYPE(MDG_MDG_DIM_COUNT(x))))0
#define MDG_ASSOC_RAW(t) void (*)(t, MDG_IC_TYPE(0))
#define MDG_ASSOC_1D(t)  void (*)(t, MDG_IC_TYPE(1))
#define MDG_ASSOC_2D(t)  void (*)(t, MDG_IC_TYPE(2))
#define MDG_ASSOC_3D(t)  void (*)(t, MDG_IC_TYPE(3))
#define MDG_ASSOC_4D(t)  void (*)(t, MDG_IC_TYPE(4))
#define MDG_ASSOC_5D(t)  void (*)(t, MDG_IC_TYPE(5))
#define MDG_ASSOC_6D(t)  void (*)(t, MDG_IC_TYPE(6))
#endif

#endif
