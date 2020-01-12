# MINIMAL REBUILD FOR THE GAME
# Use the following commands to do a minimal rebuild and flash for the game:

echo "Starting rebuild of the game..."

# Cleans the game package
ptxdist clean game
# Compiles the game package
ptxdist compile game
# Re-installs the package to the root filesystem		
ptxdist targetinstall game
# Make the root filesystem image	
ptxdist image root.romfs
# Flash the root filesystem image on the board
ptxdist test flash-rootfs

echo "...rebuild of the game finished!"