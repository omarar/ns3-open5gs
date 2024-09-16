def extract_headers(input_file, output_file):
    with open(input_file, 'r') as f:
        lines = f.readlines()

    # Extract header names
    headers = [line.strip().split('/')[-1] for line in lines]

    # Write headers to output file
    with open(output_file, 'w') as f:
        for header in headers:
            f.write(header + '\n')

if __name__ == "__main__":
    input_file = "results.txt"
    output_file = "keywords.txt"
    extract_headers(input_file, output_file)
    print("Header names extracted and saved to 'keywords.txt'")

