import os
import subprocess
import ipaddress

def create_tun_interface(interface_name, ip_address, netmask):
    # Create the TUN interface
    os.system(f'sudo ip tuntap add mode tun {interface_name}')
    
    # Bring up the interface
    os.system(f'sudo ip link set {interface_name} up')
    
    # Assign IP address to the interface
    os.system(f'sudo ip addr add {ip_address}/{netmask} dev {interface_name}')
    
    # Calculate broadcast address
    network = ipaddress.IPv4Network(f'{ip_address}/{netmask}', strict=False)
    broadcast_address = network.broadcast_address
    
    # Assign broadcast address to the interface
    os.system(f'sudo ip addr add {broadcast_address}/{netmask} broadcast {broadcast_address} dev {interface_name}')
    
    # Enable ARP on the interface
    os.system(f'sudo sysctl -w net.ipv4.conf.{interface_name}.arp_ignore=0')
    os.system(f'sudo sysctl -w net.ipv4.conf.{interface_name}.arp_announce=2')
    os.system(f'sudo sysctl -w net.ipv4.conf.{interface_name}.arp_filter=0')

if __name__ == "__main__":
    interface_name = "tun3"  # Change this to your desired interface name
    ip_address = "10.0.0.1"  # Change this to your desired IP address
    netmask = "24"  # Change this to your desired netmask
    
    create_tun_interface(interface_name, ip_address, netmask)

