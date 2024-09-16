def find_duplicates(file_path):
    # Read keywords from file
    with open(file_path, 'r') as file:
        keywords = file.readlines()

    # Strip whitespace and remove duplicates
    keywords = [keyword.strip() for keyword in keywords]
    unique_keywords = list(set(keywords))

    # Find duplicates
    duplicates = []
    for keyword in unique_keywords:
        if keywords.count(keyword) > 1:
            duplicates.append(keyword)

    return duplicates

def save_duplicates(duplicates, output_file):
    with open(output_file, 'w') as file:
        file.write("\n".join(duplicates))

def main():
    input_file = "keywords.txt"
    output_file = "duplicates.txt"

    duplicates = find_duplicates(input_file)

    if duplicates:
        print("Duplicates found:")
        print("\n".join(duplicates))
        save_duplicates(duplicates, output_file)
        print(f"Duplicates saved in {output_file}")
    else:
        print("No duplicates found.")

if __name__ == "__main__":
    main()

