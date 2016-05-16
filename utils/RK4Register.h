#ifndef COSMO_UTILS_RK4REGISTER_H
#define COSMO_UTILS_RK4REGISTER_H

#include <string>
#include <utility>
#include "Array.h"

namespace cosmo
{

template<typename IT, typename RT>
class RK4Register
{
  private:
    std::string name;
    IT points;
    RT sim_dt;

  public:
    CosmoArray _array_p;
    CosmoArray _array_a;
    CosmoArray _array_c;
    CosmoArray _array_f;

    RK4Register()
    {
      // call init()
    }

    void init(IT nx_in, IT ny_in, IT nz_in, RT sim_dt_in)
    {
      setDt(sim_dt_in);

      points = nx_in*ny_in*nz_in;
      
      _array_p.init(nx_in, ny_in, nz_in);
      _array_a.init(nx_in, ny_in, nz_in);
      _array_c.init(nx_in, ny_in, nz_in);
      _array_f.init(nx_in, ny_in, nz_in);
    }

    void setDt(RT sim_dt_in)
    {
      sim_dt = sim_dt_in;
    }

    ~RK4Register()
    {
      _array_p.~CosmoArray();
      _array_a.~CosmoArray();
      _array_c.~CosmoArray();
      _array_f.~CosmoArray();
    }

    void setName(std::string name_in)
    {
      name = name_in;
      _array_p.setName(name_in + "_p");
      _array_a.setName(name_in + "_a");
      _array_c.setName(name_in + "_c");
      _array_f.setName(name_in + "_f");
    }

    void swap_a_c()
    {
      std::swap(_array_a.nx, _array_c.nx);
      std::swap(_array_a.ny, _array_c.ny);
      std::swap(_array_a.nz, _array_c.nz);
      std::swap(_array_a.pts, _array_c.pts);
      std::swap(_array_a.name, _array_c.name);
      std::swap(_array_a._array, _array_c._array);
    }

    void swap_p_f()
    {
      std::swap(_array_p.nx, _array_f.nx);
      std::swap(_array_p.ny, _array_f.ny);
      std::swap(_array_p.nz, _array_f.nz);
      std::swap(_array_p.pts, _array_f.pts);
      std::swap(_array_p.name, _array_f.name);
      std::swap(_array_p._array, _array_f._array);
    }

    void stepInit()
    {
      for(IT i=0; i<points; ++i)
      {
        _array_a[i] = _array_p[i];
        _array_f[i] = 0;
      }
    }

    void RK1Finalize()
    {
      for(IT i=0; i<points; ++i)
      {
        _array_c[i] = _array_p[i] + sim_dt*_array_c[i]/2.0;
        _array_f[i] += _array_c[i];
      }

      swap_a_c();
    }

    void RK2Finalize()
    {
      for(IT i=0; i<points; ++i)
      {
        _array_c[i] = _array_p[i] + sim_dt*_array_c[i]/2.0;
        _array_f[i] += 2.0*_array_c[i];
      }
      
      swap_a_c();
    }

    void RK3Finalize()
    {
      for(IT i=0; i<points; ++i)
      {
        _array_c[i] = _array_p[i] + sim_dt*_array_c[i];
        _array_f[i] += _array_c[i];
      }
      
      swap_a_c();
    }

    void RK4Finalize()
    {
      for(IT i=0; i<points; ++i)
      {
        _array_f[i] = sim_dt*_array_c[i]/6.0
          + (_array_f[i] - _array_p[i])/3.0;
        _array_p[i] = _array_f[i];
      }

      swap_a_c();
    }

};

}

#endif
