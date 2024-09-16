import os

def find_files_with_core_header(root_dir):
    core_files = set()
    for dirpath, _, filenames in os.walk(root_dir):
        for filename in filenames:
            file_path = os.path.join(dirpath, filename)
            with open(file_path, 'r') as file:
                for line in file:
                    if '#include <ns3/format-inl.h>' in line:
                        core_files.add(file_path)
                        break
    return core_files

def save_files_with_core_header(core_files, output_file):
    with open(output_file, 'w') as f:
        for file_path in core_files:
            f.write(file_path + '\n')

def main():
    root_dir = '.'  # Current directory
    core_files = find_files_with_core_header(root_dir)
    save_files_with_core_header(core_files, 'corefiles.txt')
    print('List of files with "#include <ns3/core.h>" saved to corefiles.txt')

if __name__ == "__main__":
    main()

