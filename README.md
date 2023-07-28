# neutral-atom-imaging-simulation
C library and Python wrapper package for generating images of an array of neutral atoms

## Installing the Python Package
The wheel file in pip_project/dist/ can be installed using pip. It is tested for Linux and Windows 64-bit and contains the corresponding pre-built C libraries. The package can also be built from scratch to change functionality or to make it smaller if only one OS is used.

## Building
### C
The C library can be built using the existing Makefile in the top-level directory. The dll for usage in Windows can be built using the Makefile.win and MinGW-w64. While our code is written to support both MSVC and GCC, FFTW3 does not seem to like to play well with MSVC's complex value representation. Therefore, GCC is the preferred compiler here.
### Python
Using the .dll and .so versions of the C library, the Python package can be build by running

    python -m build
  
in the pip_project/ directory. Also in this directory, Doxygen can be used to generate a documentation using config.cfg
