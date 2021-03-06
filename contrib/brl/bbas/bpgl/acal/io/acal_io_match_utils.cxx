#include "acal_io_match_utils.h"

#include <iostream>

#include <vgl/io/vgl_io_point_2d.h>
#include <vsl/vsl_indent.h>


// -----acal_corr-----

//: Binary save object to stream.
void
vsl_b_write(vsl_b_ostream & os, const acal_corr& obj)
{
  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, obj.id_);
  vsl_b_write(os, obj.pt_);
}

//: Binary load object from stream.
void
vsl_b_read(vsl_b_istream & is, acal_corr& obj)
{
  if (!is) return;

  short io_version_no;
  vsl_b_read(is, io_version_no);
  switch (io_version_no)
  {
    case 1:
      vsl_b_read(is, obj.id_);
      vsl_b_read(is, obj.pt_);
      break;

    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, acal_corr&), "
                << "Unknown version number "<< io_version_no << std::endl;
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//: Output a human readable summary to the stream
void vsl_print_summary(std::ostream& os, const acal_corr& obj)
{
  os << "ID: " << obj.id_ << std::endl;
  vsl_print_summary(os, obj.pt_);
  os << std::endl;
}



// -----acal_match_pair-----

//: Binary save object to stream.
void
vsl_b_write(vsl_b_ostream & os, const acal_match_pair& obj)
{
  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, obj.corr1_);
  vsl_b_write(os, obj.corr2_);
}

//: Binary load object from stream.
void
vsl_b_read(vsl_b_istream & is, acal_match_pair& obj)
{
  if (!is) return;

  short io_version_no;
  vsl_b_read(is, io_version_no);
  switch (io_version_no)
  {
    case 1:
      vsl_b_read(is, obj.corr1_);
      vsl_b_read(is, obj.corr2_);
      break;

    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, acal_match_pair&), "
                << "Unknown version number "<< io_version_no << std::endl;
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}


//: Output a human readable summary to the stream
void vsl_print_summary(std::ostream& os, const acal_match_pair& obj)
{
  os << vsl_indent() << "Correspondence 1: " << std::endl;
  vsl_indent_inc(os);
  vsl_print_summary(os, obj.corr1_);
  vsl_indent_dec(os);
  os << vsl_indent() << "Correspondence 2: " << std::endl;
  vsl_indent_inc(os);
  vsl_print_summary(os, obj.corr2_);
  vsl_indent_dec(os);

  os << std::endl;
}
