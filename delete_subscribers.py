import subprocess

def main():
    print("WARNING: This will delete all subscribers.")
    confirmation = input("Proceed? (y/n): ")

    if confirmation.lower() == 'y':
        try:
            subprocess.run(["sudo", "./open5gs-dbctl", "reset"], check=True)
        except subprocess.CalledProcessError as e:
            print("Error executing command:", e)
    else:
        print("Operation cancelled.")

if __name__ == "__main__":
    main()

