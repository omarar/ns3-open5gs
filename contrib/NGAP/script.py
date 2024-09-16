import os

def search_keyword(keyword, directory):
    found_files = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            filepath = os.path.join(root, file)
            try:
                with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                    if keyword in content:
                        found_files.append(filepath)
            except Exception as e:
                print(f"Error reading {filepath}: {e}")
    return found_files

# Replace 'directory_path' with the path of the directory you want to search in
directory_path = '.'  # '.' represents the current directory
keyword = 'network.hpp'

found_files = search_keyword(keyword, directory_path)

if found_files:
    print("Files containing the keyword 'network.hpp':")
    for file in found_files:
        print(file)
else:
    print("No files containing the keyword 'network.hpp' found.")

