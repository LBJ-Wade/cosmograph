#ifndef COSMO_UTILS_IO_H
#define COSMO_UTILS_IO_H

#include "../cosmo_includes.h"
#include "../cosmo_types.h"
#include "../cosmo_globals.h"

#include "../utils/Fourier.h"
#include "../utils/reference_frw.h"
#include "../utils/math.h"

#include "../cosmotrace/raytrace.h"

#include "../bssn/bssn.h"
#include "../scalar/scalar.h"
#include "IOData.h"

namespace cosmo
{

void log_defines(IOData *iodata);
void io_config_backup(IOData *iodata, std::string config_file);
void io_show_progress(idx_t s, idx_t maxs);

void io_bssn_fields_snapshot(IOData *iodata, idx_t step,
  map_t & bssn_fields);
void io_bssn_fields_powerdump(IOData *iodata, idx_t step,
  map_t & bssn_fields, Fourier *fourier);
void io_bssn_constraint_violation(IOData *iodata, idx_t step, BSSN * bssnSim);
void io_bssn_dump_statistics(IOData *iodata, idx_t step,
  map_t & bssn_fields, FRW<real_t> *frw);

void io_raytrace_dump(IOData *iodata, idx_t step,
  std::vector<RayTrace<real_t, idx_t> *> const * rays);

void io_scalar_snapshot(IOData *iodata, idx_t step, Scalar * scalar);

void io_dump_2dslice(IOData *iodata, arr_t & field, std::string filename);
void io_dump_3dslice(IOData *iodata, arr_t & field, std::string filename);
void io_dump_strip(IOData *iodata, arr_t & field, std::string file,
  int axis, idx_t n1, idx_t n2);
void io_dump_value(IOData *iodata, real_t value, std::string filename,
  std::string delimiter);
void io_dump_2d_array(IOData *iodata, real_t * array, idx_t n_x, idx_t n_y,
  std::string filename, std::string dataset_name);

}

#endif