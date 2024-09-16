import os

def list_files(directory, extensions):
    files = []
    for root, _, _ in os.walk(directory):
        for extension in extensions:
            files.extend(
                [
                    os.path.join(root, file)
                    for file in os.listdir(root)
                    if file.endswith(f".{extension}")
                ]
            )
    return files

def main():
    extensions = ['h', 'cpp']

    output_file = "file_listing.txt"
    with open(output_file, 'w') as f:
        for root, _, _ in os.walk(os.getcwd()):
            files = list_files(root, extensions)
            for file in files:
                f.write(f"{file}\n")

    print(f"All file listings saved in {output_file}")

if __name__ == "__main__":
    main()


