// This is core/vil/vil_copy.h
#ifndef vil_copy_h_
#define vil_copy_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Various image copying functions.
// \author Ian Scott - Manchester
// \date 4 Oct 2002

#include <vil/vil_image_view.h>


//: Copy src to dest, without changing dest's view parameters.
// This is useful if you want to copy on image into a window on another image.
// src and dest must have identical sizes, and types.
//  O(size).
// \relates vil_image_view
template<class T>
void vil_copy_reformat(const vil_image_view<T> &src, vil_image_view<T> &dest);

//: Copy src to window in dest.
// Size of window is defined by src.
//  O(src.size).
// \relates vil_image_view
template<class T>
void vil_copy_to_window(const vil_image_view<T> &src, vil_image_view<T> &dest,
                         unsigned i0, unsigned j0);

//: Deep copy src to dest.
//  O(size).
// \relates vil_image_view
template<class T>
void vil_copy_deep(const vil_image_view<T> &src, vil_image_view<T> &dest);

//: Create a deep copy of an image, with completely new underlying memory.
//  O(size).
// \relates vil_image_view
template<class T>
vil_image_view<T> vil_copy_deep(const vil_image_view<T> &src);

#endif // vil_copy_h_
