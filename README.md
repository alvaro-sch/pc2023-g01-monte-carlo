# monte-carlo

Monte Carlo method for Photon transport simulation, based on
https://github.com/computacionparalela/tiny_mc

## Setting up the build chain

Setup llvm suite (clang, lld, ...)
```
cd ~/spack/bin/
eval `./spack load --sh llvm`
```

Setup intel suit (icx, ...)
```
cd /opt/intel/oneapi/
source setvars.sh
```
