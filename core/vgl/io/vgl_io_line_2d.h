#ifndef vgl_io_line_2d_h_
#define vgl_io_line_2d_h_
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vgl/io/vgl_line_2d.h

//:
// \file
// \date 16 Mar 2001
// \author Franck Bettinger
//
// \verbatim
// Modifications :
// 2001/03/16 Franck Bettinger    Creation
// \endverbatim

#include <vgl/vgl_line_2d.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vgl_line_2d to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const vgl_line_2d<T>& v);

//: Binary load vgl_line_2d from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vgl_line_2d<T>& v);

//: Print human readable summary of a vgl_line_2d object to a stream
template <class T>
void vsl_print_summary(vcl_ostream& os,const vgl_line_2d<T>& v);

#endif // #ifndef vgl_io_line_2d_h_
