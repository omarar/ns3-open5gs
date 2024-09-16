#script to auto add users to open5gs database launching with a big  number of users will take time but will only be done once 
import subprocess

# this is the base command to add a subscriber in open5gs using the command line 
base_command = "sudo ./open5gs-dbctl add"

# Define the initial IMSI value, this works with classic open5gs configuration 
initial_imsi = 999700000000001

# Define the number of Users to register with open5gs 
num_users = int(input("Enter the number users to registers (WARNING: can take some time for large numbers): "))

# Loop to run the command multiple times
for i in range(num_users):
    # Increment the number
    current_number = initial_imsi + i
    
    # Construct the full command
    full_command = f"{base_command} {current_number} 465B5CE8B199B49FAA5F0A2EE238A6BC E8ED289DEBA952E4283B54E88E6183CA"
    
    # Run the command using subprocess
    subprocess.run(full_command.split(), check=True)

#run the command "sudo ./open5gs-dbctl reset" to delete entire database in case of error or need to reconfig 
#the open5gs-dbctl file in the main directory can be edited to change default settings 
#you can alo check the the help command for open5gs-dbctl to check different options
