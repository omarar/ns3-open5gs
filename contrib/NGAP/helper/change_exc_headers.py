import os

def count_parent_directories():
    current_dir = os.getcwd()  # Get the current directory
    parent_count = 0  # Initialize the count of parent directories
    
    # Continue until reaching the root directory
    while True:
        current_dir, tail = os.path.split(current_dir)
        if tail == "":
            # Reached the root directory
            break
        else:
            parent_count += 1
    
    return parent_count

def generate_exceptional_headers():
    num_parents = count_parent_directories()
    x = max(0, num_parents - 3)  # Ensure x is not negative
    y = "../" * x
    
    # Open keywords.txt and exceptional_headers.txt
    with open("firstduplicates.txt", "r") as keyword_file, open("exceptional_headers.txt", "w") as outfile:
        # Process each line in keywords.txt
        for line in keyword_file:
            keyword = line.strip()  # Remove leading/trailing whitespaces
            
            # Form the exceptional header string and write it to exceptional_headers.txt
            header = f"#include \"{y}ns-3-dev/contrib/NGAP/{keyword}\"\n"
            outfile.write(header)

def replace_keywords_in_files():
    # Read keywords from keywords.txt
    with open("keywords.txt", "r") as keyword_file:
        keywords = [line.strip() for line in keyword_file]

    # Read exceptional headers from exceptional_headers.txt
    with open("exceptional_headers.txt", "r") as headers_file:
        exceptional_headers = [line.strip() for line in headers_file]

    # Iterate through all .h and .hpp files in current and subdirectories
    for root, dirs, files in os.walk('.'):
        for file in files:
            if file.endswith(('.h', '.hpp')):
                filepath = os.path.join(root, file)
                with open(filepath, 'r') as f:
                    lines = f.readlines()
                with open(filepath, 'w') as f:
                    for line in lines:
                        if line.startswith("#include"):
                            for keyword in keywords:
                                if keyword in line:
                                    # Find the corresponding exceptional header
                                    for header in exceptional_headers:
                                        if keyword in header:
                                            line = f"{header} \n"
                                            break
                                    break
                        f.write(line)

if __name__ == "__main__":
    generate_exceptional_headers()
    replace_keywords_in_files()
    print("Keywords replaced in .h and .hpp files successfully.")

