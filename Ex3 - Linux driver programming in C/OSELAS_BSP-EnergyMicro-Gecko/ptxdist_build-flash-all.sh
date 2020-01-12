# BUILDING AND FLASHING
# This file assumes that all previous configuration has been done

echo "Building all images (BootLoader+RootFS+Kernel)..."

ptxdist images

echo "...all images built, time to flash them into DK3750..."

ptxdist test flash-all

echo "...flashing all images finished!"

# These instructions will take some time, as all the image files (BootLoader + RootFS + Kernel) will be transferred to the board.