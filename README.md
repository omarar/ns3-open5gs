# ns3-open5gs
This project contains the source code and associated files for the ns-3 development environment.

# This is a branch for removing RLS

## How to build
1. Create a folder named "ns-3-dev" inside the "/home/$user" directory.
2. Copy and paste all the contents of this repository into the newly created folder "ns-3-dev".
3. Run the modified NR example "example.cc" with the command below:

~/ns-3-dev$ ./ns3 clean
~/ns-3-dev$ ./ns3 configure
~/ns-3-dev$ ./ns3 build
~/ns-3-dev$ ./ns3 run scratch/example.cc --enable-sudo

