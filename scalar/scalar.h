#ifndef COSMO_SCALAR
#define COSMO_SCALAR

#include "../cosmo_includes.h"
#include "../cosmo_types.h"
#include "../globals.h"

#include "../bssn/bssn.h"
#include "../utils/math.h"
#include "../utils/Array.h"
#include "../utils/RK4Register.h"

namespace cosmo
{

typedef struct {

  // field values
  real_t phi, Pi, psi1, psi2, psi3;

  // derivatives of fields
  real_t d1phi, d2phi, d3phi;
  real_t d1Pi, d2Pi, d3Pi;
  real_t d1psi1, d2psi1, d3psi1;
  real_t d1psi2, d2psi2, d3psi2;
  real_t d1psi3, d2psi3, d3psi3;

} ScalarData;

/** Scalar class **/
class Scalar
{
public:
  RK4Register<idx_t, real_t> phi;
  RK4Register<idx_t, real_t> Pi;
  RK4Register<idx_t, real_t> psi1;
  RK4Register<idx_t, real_t> psi2;
  RK4Register<idx_t, real_t> psi3;

  Scalar() :
    phi(), Pi(), psi1(), psi2(), psi3()
  {
    if(!USE_BSSN_SHIFT)
    {
      std::cout << "BSSN shift must be enabled.";
      throw -1;
    }
    if(USE_REFERENCE_FRW)
    {
      std::cout << "Reference FRW not supported.";
      throw -1;
    }

    std::cout << "Creating scalar class with dt=" << dt << "\n";

    phi.init(NX, NY, NZ, dt);
    Pi.init(NX, NY, NZ, dt);
    psi1.init(NX, NY, NZ, dt);
    psi2.init(NX, NY, NZ, dt);
    psi3.init(NX, NY, NZ, dt);
  }

  ~Scalar()
  {
    phi.~RK4Register();
    Pi.~RK4Register();
    psi1.~RK4Register();
    psi2.~RK4Register();
    psi3.~RK4Register();
  }

  void stepInit()
  {
    phi.stepInit();
    Pi.stepInit();
    psi1.stepInit();
    psi2.stepInit();
    psi3.stepInit();
  }

  void RK1Finalize()
  {
    phi.RK1Finalize();
    Pi.RK1Finalize();
    psi1.RK1Finalize();
    psi2.RK1Finalize();
    psi3.RK1Finalize();
  }

  void RK2Finalize()
  {
    phi.RK2Finalize();
    Pi.RK2Finalize();
    psi1.RK2Finalize();
    psi2.RK2Finalize();
    psi3.RK2Finalize();
  }

  void RK3Finalize()
  {
    phi.RK3Finalize();
    Pi.RK3Finalize();
    psi1.RK3Finalize();
    psi2.RK3Finalize();
    psi3.RK3Finalize();
  }

  void RK4Finalize()
  {
    phi.RK4Finalize();
    Pi.RK4Finalize();
    psi1.RK4Finalize();
    psi2.RK4Finalize();
    psi3.RK4Finalize();
  }

  ScalarData getScalarData(BSSNData *bd)
  {
    ScalarData sd = {0};
    idx_t i = bd->i, j = bd->j, k = bd->k;
    idx_t idx = NP_INDEX(i,j,k);

    sd.phi = phi._array_a[idx];
    sd.Pi = Pi._array_a[idx];
    sd.psi1 = psi1._array_a[idx];
    sd.psi2 = psi2._array_a[idx];
    sd.psi3 = psi3._array_a[idx];

    sd.d1phi = derivative(i, j, k, 1, phi._array_a);
    sd.d2phi = derivative(i, j, k, 2, phi._array_a);
    sd.d3phi = derivative(i, j, k, 3, phi._array_a);

    sd.d1Pi = derivative(i, j, k, 1, Pi._array_a);
    sd.d2Pi = derivative(i, j, k, 2, Pi._array_a);
    sd.d3Pi = derivative(i, j, k, 3, Pi._array_a);

    sd.d1psi1 = derivative(i, j, k, 1, psi1._array_a);
    sd.d2psi1 = derivative(i, j, k, 2, psi1._array_a);
    sd.d3psi1 = derivative(i, j, k, 3, psi1._array_a);

    sd.d1psi2 = derivative(i, j, k, 1, psi2._array_a);
    sd.d2psi2 = derivative(i, j, k, 2, psi2._array_a);
    sd.d3psi2 = derivative(i, j, k, 3, psi2._array_a);

    sd.d1psi3 = derivative(i, j, k, 1, psi3._array_a);
    sd.d2psi3 = derivative(i, j, k, 2, psi3._array_a);
    sd.d3psi3 = derivative(i, j, k, 3, psi3._array_a);

    return sd;
  }

  void RKEvolvePt(BSSNData *bd)
  {
    ScalarData sd = getScalarData(bd);
    idx_t idx = bd->idx;

    phi._array_c[idx] = dt_phi(bd, &sd);
    Pi._array_c[idx] = dt_Pi(bd, &sd);
    psi1._array_c[idx] = dt_psi1(bd, &sd);
    psi2._array_c[idx] = dt_psi2(bd, &sd);
    psi3._array_c[idx] = dt_psi3(bd, &sd);
  }

  real_t dt_phi(BSSNData *bd, ScalarData *sd)
  {
    return (
      bd->beta1*sd->d1phi + bd->beta2*sd->d2phi + bd->beta3*sd->d3phi
      - bd->alpha*sd->Pi
    );
  }

  real_t dt_Pi(BSSNData *bd, ScalarData *sd)
  {
    // return -laplacian(bd->i, bd->j, bd->k, phi._array_a);
    
    return (
      bd->beta1*sd->d1Pi + bd->beta2*sd->d2Pi + bd->beta3*sd->d3Pi
      -exp(-4.0*bd->phi)*(
        bd->gammai11*(bd->alpha*sd->d1psi1 + sd->psi1*bd->d1a) + bd->gammai12*(bd->alpha*sd->d1psi2 + sd->psi1*bd->d2a) + bd->gammai13*(bd->alpha*sd->d1psi3 + sd->psi1*bd->d3a)
        + bd->gammai21*(bd->alpha*sd->d2psi1 + sd->psi2*bd->d1a) + bd->gammai22*(bd->alpha*sd->d2psi2 + sd->psi2*bd->d2a) + bd->gammai23*(bd->alpha*sd->d2psi3 + sd->psi2*bd->d3a)
        + bd->gammai31*(bd->alpha*sd->d3psi1 + sd->psi3*bd->d1a) + bd->gammai32*(bd->alpha*sd->d3psi2 + sd->psi3*bd->d2a) + bd->gammai33*(bd->alpha*sd->d3psi3 + sd->psi3*bd->d3a)
      ) + bd->alpha*( (
          bd->Gamma1 - 2.0*(bd->gammai11*bd->d1phi + bd->gammai12*bd->d2phi + bd->gammai13*bd->d3phi)
        )*sd->psi1 + (
          bd->Gamma2 - 2.0*(bd->gammai21*bd->d1phi + bd->gammai22*bd->d2phi + bd->gammai23*bd->d3phi)
        )*sd->psi2 + (
          bd->Gamma3 - 2.0*(bd->gammai31*bd->d1phi + bd->gammai32*bd->d2phi + bd->gammai33*bd->d3phi)
        )*sd->psi3
        + bd->K*sd->Pi
        + dV(sd->phi)
      )
    );
  }

  real_t dt_psi1(BSSNData *bd, ScalarData *sd)
  {
    return (
      bd->beta1*sd->d1psi1 + bd->beta2*sd->d2psi1 + bd->beta3*sd->d3psi1
      + sd->psi1*bd->d1beta1 + sd->psi2*bd->d1beta2 + sd->psi3*bd->d1beta3
      - bd->alpha*sd->d1Pi
      - sd->Pi*bd->d1a
    );
  }

  real_t dt_psi2(BSSNData *bd, ScalarData *sd)
  {
    return (
      bd->beta1*sd->d1psi2 + bd->beta2*sd->d2psi2 + bd->beta3*sd->d3psi2
      + sd->psi1*bd->d2beta1 + sd->psi2*bd->d2beta2 + sd->psi3*bd->d2beta3
      - bd->alpha*sd->d2Pi
      - sd->Pi*bd->d2a
    );
  }

  real_t dt_psi3(BSSNData *bd, ScalarData *sd)
  {
    return (
      bd->beta1*sd->d1psi3 + bd->beta2*sd->d2psi3 + bd->beta3*sd->d3psi3
      + sd->psi1*bd->d3beta1 + sd->psi2*bd->d3beta2 + sd->psi3*bd->d3beta3
      - bd->alpha*sd->d3Pi
      - sd->Pi*bd->d3a
    );
  }

  void addBSSNSource(BSSN * bssnSim)
  {
    arr_t & DIFFr_a = *bssnSim->fields["DIFFr_a"];
    arr_t & DIFFS_a = *bssnSim->fields["DIFFS_a"];
    arr_t & S1_a = *bssnSim->fields["S1_a"];
    arr_t & S2_a = *bssnSim->fields["S2_a"];
    arr_t & S3_a = *bssnSim->fields["S3_a"];
    arr_t & S11_a = *bssnSim->fields["STF11_a"];
    arr_t & S12_a = *bssnSim->fields["STF12_a"];
    arr_t & S13_a = *bssnSim->fields["STF13_a"];
    arr_t & S22_a = *bssnSim->fields["STF22_a"];
    arr_t & S23_a = *bssnSim->fields["STF23_a"];
    arr_t & S33_a = *bssnSim->fields["STF33_a"];

    idx_t i, j, k;

    #pragma omp parallel for default(shared) private(i, j, k)
    LOOP3(i, j, k)
    {
      idx_t idx = INDEX(i,j,k);

      BSSNData bd = {0};
      // TODO: remove redundant computations here?
      bssnSim->set_paq_values(i, j, k, &bd);
      ScalarData sd = getScalarData(&bd);

      // n^mu d_mu phi
      real_t nmudmuphi = (
        dt_phi(&bd, &sd)
        - bd.beta1*sd.d1phi - bd.beta2*sd.d2phi - bd.beta3*sd.d3phi
      )/bd.alpha;
      
      // gammai^ij d_j phi d_i phi
      real_t diphidiphi = (
        bd.gammai11*sd.d1phi*sd.d1phi + bd.gammai22*sd.d2phi*sd.d2phi + bd.gammai33*sd.d3phi*sd.d3phi
        + 2.0*(bd.gammai12*sd.d1phi*sd.d2phi + bd.gammai13*sd.d1phi*sd.d3phi + bd.gammai23*sd.d2phi*sd.d3phi)
      );

      DIFFr_a[idx] = 0.5*nmudmuphi*nmudmuphi
        + 0.5*exp(-4.0*bd.phi)*diphidiphi + V(sd.phi);

      DIFFS_a[idx] = 3.0/2.0*nmudmuphi*nmudmuphi
        - 0.5*exp(-4.0*bd.phi)*diphidiphi - 3.0*V(sd.phi);

      S1_a[idx] = -exp(-4.0*bd.phi)*nmudmuphi*(bd.gamma11*sd.d1phi
        + bd.gamma12*sd.d2phi+ bd.gamma13*sd.d3phi);
      S2_a[idx] = -exp(-4.0*bd.phi)*nmudmuphi*(bd.gamma21*sd.d1phi
        + bd.gamma22*sd.d2phi+ bd.gamma23*sd.d3phi);
      S3_a[idx] = -exp(-4.0*bd.phi)*nmudmuphi*(bd.gamma31*sd.d1phi
        + bd.gamma32*sd.d2phi+ bd.gamma33*sd.d3phi);

      real_t Sij_factor = 1.0/2.0*nmudmuphi*nmudmuphi
        - 0.5*exp(-4.0*bd.phi)*diphidiphi - 1.0*V(sd.phi);

      S11_a[idx] = sd.d1phi*sd.d1phi + exp(4.0*bd.phi)*bd.gamma11*(Sij_factor);
      S12_a[idx] = sd.d1phi*sd.d2phi + exp(4.0*bd.phi)*bd.gamma12*(Sij_factor);
      S13_a[idx] = sd.d1phi*sd.d3phi + exp(4.0*bd.phi)*bd.gamma13*(Sij_factor);
      S22_a[idx] = sd.d2phi*sd.d2phi + exp(4.0*bd.phi)*bd.gamma22*(Sij_factor);
      S23_a[idx] = sd.d2phi*sd.d3phi + exp(4.0*bd.phi)*bd.gamma23*(Sij_factor);
      S33_a[idx] = sd.d3phi*sd.d3phi + exp(4.0*bd.phi)*bd.gamma33*(Sij_factor);
    }
  }

  real_t dV(real_t phi_in)
  {
    return 0;
  }

  real_t V(real_t phi_in)
  {
    return 0.2;
  }

  real_t scalarConstraint(idx_t i, idx_t j, idx_t k, idx_t dir)
  {
    switch(dir)
    {
      case 1:
        return derivative(i, j, k, dir, phi._array_a) - psi1._array_a[INDEX(i,j,k)];
      case 2:
        return derivative(i, j, k, dir, phi._array_a) - psi2._array_a[INDEX(i,j,k)];
      case 3:
        return derivative(i, j, k, dir, phi._array_a) - psi3._array_a[INDEX(i,j,k)];
    }

    throw -1;
    return 0;
  }

};

} // namespace cosmo

#endif
