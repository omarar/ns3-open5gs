import fileinput
import os

def replace_word_in_file(file_path, old_word, new_word):
    # Iterate over the lines in the file and replace the old word with the new word
    with fileinput.FileInput(file_path, inplace=True, backup='.bak') as file:
        for line in file:
            print(line.replace(old_word, new_word), end='')

    # Remove the backup file created by fileinput
    os.remove(f"{file_path}.bak")

if __name__ == "__main__":
    # Replace these values with your actual file path, old word, and new word
    file_to_modify = "/home/omar/ns-3-dev/contrib/nr/helper/annex/asn/ngap/...cc"
    
    
    old_word_to_replace = "APC_CONSTRAINED"
    new_word = "asn_per_constraint_t::APC_CONSTRAINED"
    
    #old_word_to_replace = "APC_UNCONSTRAINED"
    #new_word = "asn_per_constraint_t::APC_UNCONSTRAINED"

    replace_word_in_file(file_to_modify, old_word_to_replace, new_word)

    print(f"Replacement complete. '{old_word_to_replace}' replaced with '{new_word}' in {file_to_modify}.")
    
    
    #asn_per_constraint_t::    APC_EXTENSIBLE
    
    #asn_random_fill_result_t::   ARFILL_OK
    
    #asn_OCTET_STRING_specifics_t::  ASN_OSUBV_STR
    
    #asn_OCTET_STRING_specifics_t::  asn_OS_Subvariant

