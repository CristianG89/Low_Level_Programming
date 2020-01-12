# COMMUNICATION WITH LINUX
# Open a serial terminal emulator to communicate with the uClinux running on the board


DEV_FILE="/dev/ttyUSB0"
BAUD="115200"

# Connect to the device, if it is available...
if [ ! -e "$DEV_FILE" ]; then
    echo "Device $DEV_FILE not present..."
    exit -1
fi

miniterm.py -b "$BAUD" -p "$DEV_FILE"

echo "Connection established with device $DEV_FILE at $BAUD baudrate speed!"

# Specification of the serial communication: 115200 bps, 8 databits, 1 stop bit, No parity, No flow control