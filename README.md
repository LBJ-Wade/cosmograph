# CosmoGRaPH Code

[![Build Status](https://travis-ci.com/cwru-pat/cosmograph.svg?token=j5zJrjKFZL3UXL3HwPp6&branch=master)](https://travis-ci.com/cwru-pat/cosmograph)

## Cosmological General Relativity And (Perfect fluid | Particle) Hydrodynamics Code

*But we'll see about the "particle" part.*

### Software dependencies

 - git
 - hdf5
 - cmake
 - fftw3
 - libz
 - a compiler with c++11 support

To install these on on linux (Ubuntu, Mint), run a command like:

```{r, engine='bash', compile}
sudo apt-get install git cmake libhdf5-dev fftw3-dev libzip-dev g++
```

### Obtaining up the code
 
 - 1) Clone the code: `git clone https://github.com/cwru-pat/cosmograph`
 - 2) Change into the cloned repository directory: `cd cosmograph`

Optionally clone submodules. The code should compile and run without the
submodules, however some functionality may not exist.

 - 3) Initialize submodules: `git submodule update --init --recursive`

### Running tests

In order to make sure the code is working, you can try running the provided
tests script. This script is run by [travis-ci](https://travis-ci.com/cwru-pat/cosmograph)
as well, in an effort to detect problems introduced by changes to the code.
Once you have cloned the code and submodules, you can run:

```{r, engine='bash', run_tests}
./scripts/tests.sh
```

### Compiling the Code

In order to compile the code, you will need to create a build directory,
`cd` to this directory, run cmake, then run make. For example,

```{r, engine='bash', run_tests}
mkdir build
cd build
cmake ..
make
```

To the extent different compile-time options are available, they are
enumerated in a [cmake file](https://github.com/cwru-pat/cosmograph/blob/master/cmake/options.cmake).
You can add these by supplying an appropriate argument to cmake. For
example, to change the resolution to `N=32`, the cmake command will be

```{r, engine='bash', compile}
cmake -DCOSMO_N=32 ..
```

#### Deploy script

In the `scripts` directory, a `deploy_runs.sh` bash script exists to help
deploy BSSN+dust runs quickly. Run `deploy_runs.sh --help` for some usage
details.

### Running the code

The generated executable should accept a single configuration file as a
parameter. The configuration parameter contains various runtime options that
can be  updated without needing to recompile. For example, in order to run a
'test' simulation of a pure-dust universe, the corresponding command would be

```{r, engine='bash', compile}
./cosmo ../config/dust_test.txt
```

### Viewing output

A Mathematica notebook is available (`plots.nb`) demonstrating some basic
processing of output files for plotting. In order to understand what is in a
given output file, you may need to look at the `IO/io.cc` file.

### Support

CosmoGRaPH is under heavy development, and as such, all functionality is not 
only subject to change; but no support is currently offered. You are
nevertheless welcome to use and modify the code in its present form.

Doxygen documentation can be generated by running
```{r, engine='bash', compile}
doxygen docs/Doxyfile
```
from the main directory. Additional documentation and information may be
available in the `docs` directory, but this is not guaranteed to be
up-to-date. 'Collaboration diagrams' can also be generated for the code
by doxygen provided the `graphviz` library is available. To install this
on Ubuntu systems, a command like `sudo apt-get install graphviz` should
work.
