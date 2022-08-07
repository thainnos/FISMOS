# Create Container
fallocate -l 24MB myLUKScontainer

# Obtain key from FISMOS
echo 729e83df [...] 232126e8f958cba2 >  /dev/fismos_data_buffer
echo df00000f > /dev/fismos_instruction_register
cat /dev/fismos_data_buffer > luksFISMOSkey

# Use key from FISMOS for setting up container
cryptsetup -v luksFormat myLUKScontainer --key-file ~/luksFISMOSkey

# Unlock container with keyfile from FISMOS
cryptsetup -v luksFormat myLUKScontainer --key-file ~/luksFISMOSkey
mount /dev/mapper/container /mnt/container
