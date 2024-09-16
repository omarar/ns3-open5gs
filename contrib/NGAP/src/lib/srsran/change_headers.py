import os
import re

# Function to read keywords from the file
def read_keywords(filename):
    with open(filename, 'r') as f:
        return set(line.strip() for line in f)

# Function to process each file
def process_file(file_path, keywords):
    # Read the file
    with open(file_path, 'r') as f:
        lines = f.readlines()

    # Flag to track if changes were made
    changes_made = False

    # Regex pattern to match lines starting with #include
    include_pattern = re.compile(r'^\s*#include\s+"(.*?)"')

    # Iterate through each line
    for i, line in enumerate(lines):
        match = include_pattern.match(line)
        if match:
            # Extract the header filename
            header = match.group(1)
            # Check if any keyword exists in the header
            if any(keyword in header for keyword in keywords):
                # Modify the line to required format
                new_line = '#include <ns3/' + header.split('/')[-1] + '>\n'
                lines[i] = new_line
                changes_made = True

    # If changes were made, write the modified lines back to the file
    if changes_made:
        with open(file_path, 'w') as f:
            f.writelines(lines)

# Function to traverse directories recursively
def traverse_directories(root_dir, keywords):
    for dirpath, dirnames, filenames in os.walk(root_dir):
        # Iterate through each file in the directory
        for file in filenames:
            # Check if it's a .cc or .h file
            if file.endswith(('.cc', '.h')):
                # Process the file
                process_file(os.path.join(dirpath, file), keywords)

# Main function
def main():
    # Read keywords from the file
    keywords = read_keywords('externalfiles.txt')

    # Traverse the current directory recursively
    root_dir = '.'
    traverse_directories(root_dir, keywords)

# Run the main function
if __name__ == "__main__":
    main()

