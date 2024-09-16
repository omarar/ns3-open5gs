import os

def replace_include_statement(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    replaced = False
    with open(file_path, 'w') as file:
        for line in lines:
            if '#include <ns3/format.h>' in line:
                file.write('#include "../../contrib/NGAP/src/external/fmt/include/fmt/format.h"\n')
                replaced = True
            else:
                file.write(line)

    return replaced

def process_core_files(corefiles_path):
    current_directory = os.getcwd()
    with open(corefiles_path, 'r') as corefiles:
        for line in corefiles:
            file_path = line.strip()
            absolute_path = os.path.join(current_directory, file_path)
            if os.path.exists(absolute_path):
                replaced = replace_include_statement(absolute_path)
                if replaced:
                    print(f"Replaced include statement in: {absolute_path}")
            else:
                print(f"File not found: {absolute_path}")

if __name__ == "__main__":
    corefiles_path = "corefiles.txt"
    process_core_files(corefiles_path)

