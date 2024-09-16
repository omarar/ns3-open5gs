def delete_first_44_chars(input_file, output_file):
    with open(input_file, 'r') as f:
        lines = f.readlines()

    with open(output_file, 'w') as f:
        for line in lines:
            new_line = line[40:]
            f.write(new_line)

if __name__ == "__main__":
    input_file = "file_listing.txt"
    output_file = "results.txt"
    delete_first_44_chars(input_file, output_file)

