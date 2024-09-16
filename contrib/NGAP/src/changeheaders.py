import os
import re

def read_keywords(filename):
    with open(filename, 'r') as f:
        keywords = [line.strip() for line in f]
    return keywords

def process_files(directory, keywords):
    include_regex = re.compile(r'#include\s*[<"]([^>"]+)[>"]')
    
    # Traverse directory and subdirectories
    for root, _, files in os.walk(directory):
        for file_name in files:
            file_path = os.path.join(root, file_name)
            with open(file_path, 'r') as f:
                lines = f.readlines()
            
            modified_lines = []
            for line in lines:
                match = include_regex.match(line)
                if match:
                    included_file = match.group(1)
                    for keyword in keywords:
                        if keyword in included_file:
                            new_include = f'#include <ns3/{keyword}>'
                            modified_lines.append(new_include + '\n')
                            break
                    else:
                        modified_lines.append(line)
                else:
                    modified_lines.append(line)
            
            with open(file_path, 'w') as f:
                f.writelines(modified_lines)

if __name__ == "__main__":
    keyword_file = "rlckeywords.txt"
    keywords = read_keywords(keyword_file)
    
    directory = "rlc"
    process_files(directory, keywords)

