import os
import re

# Function to search for headers in a file
def search_headers(file_path, headers_set):
    with open(file_path, 'r') as file:
        content = file.read()
        headers = re.findall(r'#include\s+["<](.+?)[">]', content)
        for header in headers:
            headers_set.add(header)

# Function to traverse directories and search for headers
def traverse_directories(root_dir, headers_set):
    for dirpath, _, filenames in os.walk(root_dir):
        for filename in filenames:
            file_path = os.path.join(dirpath, filename)
            if filename.endswith('.h') or filename.endswith('.hpp') or filename.endswith('.c') or filename.endswith('.cpp'):
                search_headers(file_path, headers_set)

# Main function to execute the search
def main():
    headers_set = set()
    current_directory = os.getcwd()
    traverse_directories(current_directory, headers_set)

    with open('headers.txt', 'w') as output_file:
        for header in headers_set:
            output_file.write(header + '\n')

if __name__ == "__main__":
    main()

