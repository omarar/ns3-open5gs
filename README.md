# ns-3 with open5gs Integration

This repository provides instructions and scripts for integrating ns-3 with open5gs. Installing open5gs is a prerequisite. This version assumes that the RAN and the Core are on two different machines.

## Setup Instructions

Follow these steps to configure and build ns-3 with open5gs integration:

1. Configure ns-3:
    ```bash
    ./ns3 configure --enable-examples --enable-tests
    ```

2. Build ns-3:
    ```bash
    ./ns3 build
    ```

## Subscriber Management

Subscriber management should be done on the machine where open5gs is installed and managed. The tool open5gs-dbctl should be in the same directory as the following scripts for them to function properly.

### Adding Subscribers

An automated script `add_subscribers.py` is provided to register users to the open5gs database. This script will take some time for a large number of subscribers but only needs to be run once. Refer to the script contents for more details.

### Deleting Subscribers

To delete all subscriber data from the open5gs database, use the script `delete_subscribers.py`.

## Integration with open5gs

To integrate ns-3 with open5gs, follow these steps:

1. Review `example.cc` in the scratch subdirectory to understand how to add open5gs related commands.

2. Note that the UE-BS attachment command used by ns-3 should be written before open5gs related commands. To avoid conflicts, functions to attach to open5gs could be called separately as demonstrated in `grid_example.cc` in the scratch subdirectory.

## Running Examples

Ensure to run examples with sudo permission by enabling the sudo flag:

```bash
./ns3 run example.cc --enable-sudo

