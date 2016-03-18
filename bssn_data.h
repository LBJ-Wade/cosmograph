#ifndef COSMO_BSSN_DATA
#define COSMO_BSSN_DATA

#include "cosmo_macros.h"
#include "bssn_macros.h"

namespace cosmo
{

typedef struct {

  idx_t i, j, k, idx;

  // local copies of current field values
  BSSN_APPLY_TO_FIELDS(DECLARE_REAL_T)
  // Source terms
  BSSN_APPLY_TO_SOURCES(DECLARE_REAL_T)
  // "extra" fields
  BSSN_APPLY_TO_GEN1_EXTRAS(DECLARE_REAL_T)

  // non-differenced quantities
  real_t phi, K, r, S, alpha;
  real_t gamma11, gamma12, gamma13, gamma22, gamma23, gamma33;
  real_t gammai11, gammai12, gammai13, gammai22, gammai23, gammai33;

  // generic var for misc. expressions
  real_t trace, expression;

  // ricci tensor components
  real_t ricci11, ricci12, ricci13, ricci22, ricci23, ricci33;
  real_t ricciTF11, ricciTF12, ricciTF13, ricciTF22, ricciTF23, ricciTF33;
  real_t Uricci11, Uricci12, Uricci13, Uricci22, Uricci23, Uricci33;
  real_t unitRicci; // ricci scalar
  // for constraint checking

  // derivatives of \alpha
    // covariant double-derivatives 
    real_t D1D1aTF, D1D2aTF, D1D3aTF, D2D2aTF, D2D3aTF, D3D3aTF;
    real_t DDaTR;
    // normal derivatives of
    real_t d1a, d2a, d3a;

  // derivatives of phi
    // covariant double-derivatives 
    real_t D1D1phi, D1D2phi, D1D3phi, D2D2phi, D2D3phi, D3D3phi;
    // normal derivatives of
    real_t d1phi, d2phi, d3phi;
    real_t d1d1phi, d1d2phi, d1d3phi, d2d2phi, d2d3phi, d3d3phi;

  // ders of K
  real_t d1K, d2K, d3K;

  // Contravariant (upstairs index) ext. curvature
  real_t Acont11, Acont12, Acont13, Acont22, Acont23, Acont33;

  // Christoffel symbols
  real_t G111, G112, G113, G122, G123, G133,
         G211, G212, G213, G222, G223, G233,
         G311, G312, G313, G322, G323, G333;

  // Lowered index christoffel symbols
  real_t GL111, GL112, GL113, GL122, GL123, GL133,
         GL211, GL212, GL213, GL222, GL223, GL233,
         GL311, GL312, GL313, GL322, GL323, GL333;

  // contraction of christoffel symbols ("Gamma_d" in Z4c)
  real_t Gammad1, Gammad2, Gammad3;

  // derivatives of the metric, d_i g_jk
  real_t d1g11, d1g12, d1g13, d1g22, d1g23, d1g33,
         d2g11, d2g12, d2g13, d2g22, d2g23, d2g33,
         d3g11, d3g12, d3g13, d3g22, d3g23, d3g33;

  // second derivatives of the metric d_i d_j g_kl
  real_t d1d1g11, d1d1g12, d1d1g13, d1d1g22, d1d1g23, d1d1g33,
         d1d2g11, d1d2g12, d1d2g13, d1d2g22, d1d2g23, d1d2g33,
         d1d3g11, d1d3g12, d1d3g13, d1d3g22, d1d3g23, d1d3g33,
         d2d2g11, d2d2g12, d2d2g13, d2d2g22, d2d2g23, d2d2g33,
         d2d3g11, d2d3g12, d2d3g13, d2d3g22, d2d3g23, d2d3g33,
         d3d3g11, d3d3g12, d3d3g13, d3d3g22, d3d3g23, d3d3g33;

  // Full metric ("m") and inverse ("mi") (needed for fluid)
  real_t m00, m01, m02, m03, m11, m12, m13, m22, m23, m33;
  real_t mi00, mi01, mi02, mi03, mi11, mi12, mi13, mi22, mi23, mi33;

  // derivatives of full metric ("m") (needed for fluid)
  real_t d1m00, d1m01, d1m02, d1m03, d1m11, d1m12, d1m13, d1m22, d1m23, d1m33,
         d2m00, d2m01, d2m02, d2m03, d2m11, d2m12, d2m13, d2m22, d2m23, d2m33,
         d3m00, d3m01, d3m02, d3m03, d3m11, d3m12, d3m13, d3m22, d3m23, d3m33;

  // H constraint calc.
  real_t H;
  // Misc. debugging calc
  real_t db;

  real_t d1theta, d2theta, d3theta;
  // additional variables to handle absence of Z4c terms in macros
  // (make sure these get initialized to 0!)
  #if !USE_Z4c_DAMPING
    real_t theta;
  #endif

  // additional variables to handle absence of shift terms in macros
  // (make sure these get initialized to 0!)
  real_t d1beta1, d2beta1, d3beta1;
  real_t d1beta2, d2beta2, d3beta2;
  real_t d1beta3, d2beta3, d3beta3;
  #if !USE_BSSN_SHIFT
    real_t beta1;
    real_t beta2;
    real_t beta3;
  #endif

  // Reference FRW quantities
  real_t phi_FRW;
  real_t K_FRW;
  real_t rho_FRW, S_FRW;

} BSSNData;

} /* namespace cosmo */

#endif
