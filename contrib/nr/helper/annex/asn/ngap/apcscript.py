import os

def replace_words_in_file(file_path, old_word, new_word):
    with open(file_path, 'r') as file:
        file_content = file.read()

    modified_content = file_content.replace(old_word, new_word)

    with open(file_path, 'w') as file:
        file.write(modified_content)

def main():
    files_to_process = [
        "ASN_NGAP_UnavailableGUAMIList.cc",
        "ASN_NGAP_EUTRAencryptionAlgorithms.cc",
        # Add more file names as needed
    ]

    old_word_to_replace1 = "APC_CONSTRAINED"
    new_word1 = "asn_per_constraint_t::APC_CONSTRAINED"

    old_word_to_replace2 = "APC_UNCONSTRAINED"
    new_word2 = "asn_per_constraint_t::APC_UNCONSTRAINED"
    
    old_word_to_replace3 = "APC_EXTENSIBLE"
    new_word3 = "asn_per_constraint_t::APC_EXTENSIBLE"
    
    old_word_to_replace4 = "APC_SEMI_CONSTRAINED"
    new_word4 = "asn_per_constraint_t::APC_SEMI_CONSTRAINED"
    
     
    directory_path = "/home/omar/ns-3-dev/contrib/nr/helper/annex/asn/ngap/"

    for file_name in files_to_process:
        file_path = os.path.join(directory_path, file_name)
        
        replace_words_in_file(file_path, old_word_to_replace1, new_word1)
        replace_words_in_file(file_path, old_word_to_replace2, new_word2)
        replace_words_in_file(file_path, old_word_to_replace3, new_word3)
        replace_words_in_file(file_path, old_word_to_replace4, new_word4)

if __name__ == "__main__":
    main()

