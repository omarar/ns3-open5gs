def find_duplicates(file1_path, file2_path):
    # Read keywords from file1
    with open(file1_path, 'r') as file1:
        keywords1 = file1.readlines()

    # Read keywords from file2
    with open(file2_path, 'r') as file2:
        keywords2 = file2.readlines()

    # Strip whitespace and remove duplicates
    keywords1 = [keyword.strip() for keyword in keywords1]
    keywords2 = [keyword.strip() for keyword in keywords2]

    # Find duplicates
    duplicates = list(set(keywords1) & set(keywords2))

    return duplicates

def save_duplicates(duplicates, output_file):
    with open(output_file, 'w') as file:
        file.write("\n".join(duplicates))

def main():
    input_file1 = "keywords.txt"
    input_file2 = "keywords2.txt"
    output_file = "duplicates2.txt"

    duplicates = find_duplicates(input_file1, input_file2)

    if duplicates:
        print("Duplicates found:")
        print("\n".join(duplicates))
        save_duplicates(duplicates, output_file)
        print(f"Duplicates saved in {output_file}")
    else:
        print("No duplicates found.")

if __name__ == "__main__":
    main()

