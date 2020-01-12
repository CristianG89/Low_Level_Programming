# MINIMAL REBUILD FOR THE DRIVER
# Use the following commands to do a minimal rebuild and flash for the driver:

echo "Starting rebuild of the driver..."

# Cleans the gamepad package
ptxdist clean driver-gamepad
# Compiles the gamepad package
ptxdist compile driver-gamepad
# Re-installs the package to the root filesystem
ptxdist targetinstall driver-gamepad
# Re-installs the kernel package to the root filesystem	
ptxdist targetinstall kernel
# Make the root filesystem image
ptxdist image root.romfs
# Flash the root filesystem image on the board
ptxdist test flash-rootfs

echo "...rebuild of the driver finished!"