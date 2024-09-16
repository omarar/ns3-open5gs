import os
import re

def find_header_names(directory):
    header_names = set()
    header_regex = re.compile(r'#include\s*[<"]([^>"]+)[>"]')

    # Traverse directory and subdirectories
    for root, _, files in os.walk(directory):
        for file_name in files:
            if file_name.endswith('.h') or file_name.endswith('.hpp') or file_name.endswith('.hh'):
                file_path = os.path.join(root, file_name)
                with open(file_path, 'r') as file:
                    content = file.read()
                    matches = header_regex.findall(content)
                    header_names.update(matches)

    return header_names

if __name__ == "__main__":
    directory = "rlc"  # Change this to your directory path
    header_names = find_header_names(directory)
    
    # Save header names to a file
    with open("rlcheaders.txt", "w") as f:
        for header_name in header_names:
            f.write(header_name + '\n')

