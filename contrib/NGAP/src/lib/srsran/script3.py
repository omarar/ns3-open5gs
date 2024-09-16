import os
import re

def generate_keyword_list(keyword_file):
    with open(keyword_file, 'r') as f:
        keywords = [line.strip() for line in f.readlines()]
    return keywords

def replace_include_with_keyword(file_path, keywords):
    with open(file_path, 'r') as f:
        lines = f.readlines()

    modified_lines = []
    for line in lines:
        if '#include' in line:
            for keyword in keywords:
                if keyword in line:
                    modified_lines.append('#include <ns3/{}>\n'.format(keyword))
                    break
            else:
                modified_lines.append(line)
        else:
            modified_lines.append(line)

    with open(file_path, 'w') as f:
        f.writelines(modified_lines)

def process_files_in_directory(directory, keywords):
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.h') or file.endswith('.cpp'):
                file_path = os.path.join(root, file)
                replace_include_with_keyword(file_path, keywords)

if __name__ == "__main__":
    keyword_file = "keywords.txt"
    keywords = generate_keyword_list(keyword_file)

    folders_to_search = ['pdcp', 'security', 'support', 'instrumentation','adt','ran','srslog']
    for folder in folders_to_search:
        process_files_in_directory(folder, keywords)

    print("Replacement completed successfully.")

