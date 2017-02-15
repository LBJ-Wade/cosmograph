#include "static_ic.h"
#include "../../cosmo_includes.h"
#include "../../cosmo_types.h"
#include "../../cosmo_globals.h"
#include "../../ICs/ICs.h"
#include "../../utils/math.h"

#include <boost/math/special_functions/spherical_harmonic.hpp>

#include <utility>
#include <random>

namespace cosmo
{

typedef std::pair<real_t, real_t> complex_t;
#define m_idx(l, m) ((l)+(m))


/**
 * @brief Gaussian random field ICs
 */
void dust_ic_set_random(BSSN * bssn, Static * dust, Fourier * fourier,
  IOData * iodata)
{
  idx_t i, j, k;

  arr_t & DIFFr_a = *bssn->fields["DIFFr_a"];
  arr_t & DIFFphi_p = *bssn->fields["DIFFphi_p"];
  arr_t & DIFFphi_a = *bssn->fields["DIFFphi_a"];
  arr_t & DIFFphi_f = *bssn->fields["DIFFphi_f"];

  arr_t & DIFFD_a = *dust->fields["DIFFD_a"];

  auto & frw = bssn->frw;

  ICsData icd = cosmo_get_ICsData();
  iodata->log( "Generating ICs with peak at k = " + stringify(icd.peak_k) );
  iodata->log( "Generating ICs with peak amp. = " + stringify(icd.peak_amplitude) );

  // the conformal factor in front of metric is the solution to
  // d^2 exp(\phi) = -2*pi exp(5\phi) * \delta_rho
  // generate gaussian random field 1 + xi = exp(phi) (store xi in phi_p temporarily):
  set_gaussian_random_field(DIFFphi_p, fourier, &icd);

  // delta_rho = -lap(phi)/(1+xi)^5/2pi
# pragma omp parallel for default(shared) private(i,j,k)
  LOOP3(i,j,k) {
    DIFFr_a[NP_INDEX(i,j,k)] = -0.5/PI/(
      pow(1.0 + DIFFphi_p[NP_INDEX(i,j,k)], 5.0)
    )*(
      double_derivative(i, j, k, 1, 1, DIFFphi_p)
      + double_derivative(i, j, k, 2, 2, DIFFphi_p)
      + double_derivative(i, j, k, 3, 3, DIFFphi_p)
    );
  }

  // phi = ln(xi)
# pragma omp parallel for default(shared) private(i,j,k)
  LOOP3(i,j,k) {
    idx_t idx = NP_INDEX(i,j,k);
    DIFFphi_a[idx] = log1p(DIFFphi_p[idx]);
    DIFFphi_f[idx] = log1p(DIFFphi_p[idx]);
    DIFFphi_p[idx] = log1p(DIFFphi_p[idx]);
  }

  // Make sure min density value > 0
  // Set conserved density variable field
  real_t min = icd.rho_K_matter;
  real_t max = min;
  LOOP3(i,j,k)
  {
    idx_t idx = NP_INDEX(i,j,k);
    real_t rho_FRW = icd.rho_K_matter;
    real_t DIFFr = DIFFr_a[idx];
    real_t rho = rho_FRW + DIFFr;
    // phi_FRW = 0
    real_t DIFFphi = DIFFphi_a[idx];
    // phi = DIFFphi
    // DIFFK = 0

    DIFFD_a[idx] =
      rho_FRW*expm1(6.0*DIFFphi) + exp(6.0*DIFFphi)*DIFFr;

    if(rho < min)
    {
      min = rho;
    }
    if(rho > max)
    {
      max = rho;
    }
    if(rho != rho)
    {
      iodata->log("Error: NaN energy density.");
      throw -1;
    }
  }

  iodata->log( "Minimum fluid density: " + stringify(min) );
  iodata->log( "Maximum fluid density: " + stringify(max) );
  iodata->log( "Average fluctuation density: " + stringify(average(DIFFD_a)) );
  iodata->log( "Std.dev fluctuation density: " + stringify(standard_deviation(DIFFD_a)) );
  if(min < 0.0)
  {
    iodata->log("Error: negative density in some regions.");
    throw -1;
  }

# if USE_REFERENCE_FRW
  // Set values in reference FRW integrator
  real_t rho_FRW = icd.rho_K_matter;
  real_t K_frw = -sqrt(24.0*PI*rho_FRW);

  frw->set_phi(0.0);
  frw->set_K(K_frw);
  frw->addFluid(rho_FRW, 0.0 /* w=0 */);
# else
  arr_t & DIFFK_p = *bssn->fields["DIFFK_p"];
  arr_t & DIFFK_a = *bssn->fields["DIFFK_a"];
  // add in FRW pieces to ICs
  // phi is unchanged
  // rho (D) and K get contribs
  // w=0 fluid only
# pragma omp parallel for default(shared) private(i,j,k)
  LOOP3(i,j,k)
  {
    idx_t idx = NP_INDEX(i,j,k);
    real_t rho_FRW = icd.rho_K_matter;
    real_t D_FRW = rho_FRW; // on initial slice

    DIFFr_a[idx] += rho_FRW;

    DIFFK_a[idx] = -sqrt(24.0*PI*rho_FRW);
    DIFFK_p[idx] = -sqrt(24.0*PI*rho_FRW);

    DIFFD_a[idx] += D_FRW;
  }
# endif
}



/**
 * @brief Spherical "shell" of perturbations around an observer
 */
void dust_ic_set_sphere(BSSN * bssn, Static * dust, IOData * iodata)
{
  idx_t i, j, k;

  arr_t & DIFFr_a = *bssn->fields["DIFFr_a"];
  arr_t & DIFFphi_p = *bssn->fields["DIFFphi_p"];
  arr_t & DIFFphi_a = *bssn->fields["DIFFphi_a"];
  arr_t & DIFFphi_f = *bssn->fields["DIFFphi_f"];

  arr_t & DIFFD_a = *dust->fields["DIFFD_a"];

  auto & frw = bssn->frw;


  // shell amplitude
  real_t A = stod(_config("shell_amplitude", "1e-5"));
  // Shell described by only one fixed l:
  idx_t l = stoi(_config("shell_angular_scale_l", "1"));
  iodata->log( "Generating ICs with shell angular scale of l = " + stringify(l) );
  iodata->log( "Generating ICs with peak amp. = " + stringify(A) );

  // spherical shell of perturbations in phi0field

  // place shell around center of box
  real_t x0 = (NX - 0.5)*dx/2.0;
  real_t y0 = (NY - 0.5)*dx/2.0;
  real_t z0 = (NZ - 0.5)*dx/2.0;

  // place spherical "shell" of fluctuations at r = NX/6, 1/3-way between observer and boundary 
  real_t r_shell = NX*dx / 5.0;
  // shell width
  real_t shell_width = NX*dx / 10.0; // 4-sigma limit for gaussian

  // Angular fluctuations in shell described by spherical harmonic coeffs, a_lm's,
  complex_t * alms = new complex_t[m_idx(l,l)+1];
  std::mt19937 gen(7);
  std::normal_distribution<> normal_dist(0,1);
  std::uniform_real_distribution<> uniform_dist(0.0, 2.0*PI);
std::cout << "normal_dist(gen) = " << normal_dist(gen) << ", uniform_dist(gen) = " << uniform_dist(gen) << "\n";

  // zero mode:
  alms[m_idx(l, 0)].first = normal_dist(gen);
  alms[m_idx(l, 0)].second = 0;
  // positive modes:
  for(int m = 1; m <= l; m++)
  {
    real_t phase = uniform_dist(gen);
    real_t amp = normal_dist(gen);
    alms[m_idx(l,m)].first = amp*std::cos(phase);
    alms[m_idx(l,m)].second = amp*std::sin(phase);
  }
  // negative modes:
  for(int m = -l; m <= -1; m++)
  {
    real_t Condon_Shortley_phase = std::abs(m) % 2 ? 1 : -1;
    alms[m_idx(l,m)].first = alms[m_idx(l,-m)].first*Condon_Shortley_phase;
    alms[m_idx(l,m)].second = -alms[m_idx(l,-m)].second*Condon_Shortley_phase;
  }

std::cout << "al-2_r = " << alms[m_idx(l,-2)].first << ", al-2_i = " << alms[m_idx(l,-2)].second << "\n";

  LOOP3(i,j,k) {
    idx_t idx = NP_INDEX(i,j,k);

    real_t x = i*dx;
    real_t y = j*dx;
    real_t z = k*dx;

    real_t r = sqrt( pw2(x - x0) + pw2(y - y0) + pw2(z - z0) );
    real_t theta = acos((z - z0) / r); // "theta" (polar angle)
    real_t phi = atan2(y - y0, x - x0); // "phi" (azimuthal angle)

    real_t DIFFphi_r = 0.0;
    real_t DIFFphi_i = 0.0;
    for(int m=-l; m<=l; m++)
    {
      real_t Y_r = boost::math::spherical_harmonic_r(l, m, theta, phi);
      real_t Y_i = boost::math::spherical_harmonic_i(l, m, theta, phi);

      DIFFphi_r += alms[m_idx(l,m)].first * Y_r;
      DIFFphi_i += alms[m_idx(l,m)].second * Y_i;
    }
    if(std::abs(DIFFphi_i) > 1e-6)
    {
      iodata->log("Significant non-zero imaginary component of solution exists!");
      throw -1;
    }

    // gaussian profile shell of fluctuations
    real_t U_r = A*std::exp( -pw2((r - r_shell)/2.0/(4.0*shell_width)) );
    // cosine profile
    // real_t U_r = (r < r_shell-shell_width || r > r_shell+shell_width ) ? 0 : A*(1+std::cos(PI*(r-r_shell)/shell_width));

    DIFFphi_p[idx] = U_r*DIFFphi_r;


    if(i==NX/2 && j==NY/2 && k==NZ/2)
      std::cout << "At a grid point close to the middle, r = " << r
                << ", r_shell = " << r_shell
                << ", shell_width = " << shell_width
                << ", U_r = " << U_r
                << ", DIFFphi_p = " << DIFFphi_p[idx] << "\n";
  }
  // cleanup
  delete [] alms;

  // delta_rho = -lap(phi)/(1+xi)^5/2pi
# pragma omp parallel for default(shared) private(i,j,k)
  LOOP3(i,j,k) {
    DIFFr_a[NP_INDEX(i,j,k)] = -0.5/PI/(
      pow(1.0 + DIFFphi_p[NP_INDEX(i,j,k)], 5.0)
    )*(
      double_derivative(i, j, k, 1, 1, DIFFphi_p)
      + double_derivative(i, j, k, 2, 2, DIFFphi_p)
      + double_derivative(i, j, k, 3, 3, DIFFphi_p)
    );
  }

  // phi = ln(xi)
# pragma omp parallel for default(shared) private(i,j,k)
  LOOP3(i,j,k) {
    idx_t idx = NP_INDEX(i,j,k);
    DIFFphi_a[idx] = log1p(DIFFphi_p[idx]);
    DIFFphi_f[idx] = log1p(DIFFphi_p[idx]);
    DIFFphi_p[idx] = log1p(DIFFphi_p[idx]);
  }

  // Make sure min density value > 0
  // Set conserved density variable field
  real_t min = 3.0/PI/8.0;
  real_t max = min;
  LOOP3(i,j,k)
  {
    idx_t idx = NP_INDEX(i,j,k);
    real_t rho_FRW = 3.0/PI/8.0;
    real_t DIFFr = DIFFr_a[idx];
    real_t rho = rho_FRW + DIFFr;
    // phi_FRW = 0
    real_t DIFFphi = DIFFphi_a[idx];
    // phi = DIFFphi
    // DIFFK = 0

    DIFFD_a[idx] =
      rho_FRW*expm1(6.0*DIFFphi) + exp(6.0*DIFFphi)*DIFFr;

    if(rho < min)
    {
      min = rho;
    }
    if(rho > max)
    {
      max = rho;
    }
    if(rho != rho)
    {
      iodata->log("Error: NaN energy density.");
      throw -1;
    }
  }

  iodata->log( "Minimum fluid density: " + stringify(min) );
  iodata->log( "Maximum fluid density: " + stringify(max) );
  iodata->log( "Average fluctuation density: " + stringify(average(DIFFD_a)) );
  iodata->log( "Std.dev fluctuation density: " + stringify(standard_deviation(DIFFD_a)) );
  if(min < 0.0)
  {
    iodata->log("Error: negative density in some regions.");
    throw -1;
  }

# if USE_REFERENCE_FRW
  // Set values in reference FRW integrator
  real_t rho_FRW = 3.0/PI/8.0;
  real_t K_frw = -sqrt(24.0*PI*rho_FRW);

  frw->set_phi(0.0);
  frw->set_K(K_frw);
  frw->addFluid(rho_FRW, 0.0 /* w=0 */);
# else
  arr_t & DIFFK_p = *bssn->fields["DIFFK_p"];
  arr_t & DIFFK_a = *bssn->fields["DIFFK_a"];
  // add in FRW pieces to ICs
  // phi is unchanged
  // rho (D) and K get contribs
  // w=0 fluid only
# pragma omp parallel for default(shared) private(i,j,k)
  LOOP3(i,j,k)
  {
    idx_t idx = NP_INDEX(i,j,k);
    real_t rho_FRW = icd.rho_K_matter;
    real_t D_FRW = rho_FRW; // on initial slice

    DIFFr_a[idx] += rho_FRW;

    DIFFK_a[idx] = -sqrt(24.0*PI*rho_FRW);
    DIFFK_p[idx] = -sqrt(24.0*PI*rho_FRW);

    DIFFD_a[idx] += D_FRW;
  }
# endif
}

}
