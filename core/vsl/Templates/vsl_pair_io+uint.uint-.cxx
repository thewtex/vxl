#include "vsl/vsl_pair_io.hxx"
#include "vsl/vsl_map_io.hxx"
VSL_PAIR_IO_INSTANTIATE(unsigned int, unsigned int);
VSL_MAP_IO_INSTANTIATE(unsigned, unsigned, std::less<unsigned>);
