#ifndef COSMO_STATIC
#define COSMO_STATIC

#include "cosmo.h"
#include "globals.h"

namespace cosmo
{

/** Static matter class **/
class Static
{
  /* Fluid field */
  // just a density variable
  GEN1_ARRAY_CREATE(D);

public:
  std::map <std::string, real_t *> fields;

  Static();
  ~Static();

  void addBSSNSrc(std::map <std::string, real_t *> & bssn_fields);

  void init();
};

}

#endif