# SETTING UP THE PROJECT
# Issue the following commands to set up the ptxdist project:
# These operations tell ptxdist where to find the necessary configuration files and the toolchain in order to build the exercise.

echo "Setting up the project..."

# cd "OSELAS_BSP-EnergyMicro-Gecko"		Not necessary as file is already in the right place...
ptxdist select "configs/ptxconfig"
ptxdist platform "configs/platform-energymicro-efm32gg-dk3750/platformconfig"
ptxdist toolchain "/opt/ex3/OSELAS.Toolchain-2012.12.0/arm-cortexm3-uclinuxeabi/gcc-4.7.2-uclibc-0.9.33.2-binutils-2.22-kernel-3.6-sanitized/bin"

echo "...project set up finished!"

# toolchain path depends on where the toolchain is installed (for example under /opt/ex3/). Check README file in the top directory for the exact value.