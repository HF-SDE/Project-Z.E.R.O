import subprocess
import os
import sys

def install_packages():
    """Install required packages automatically."""
    try:
        print("Installing required packages...")
        script_dir = os.path.dirname(os.path.abspath(__file__))
        parent_dir = os.path.dirname(script_dir)  # Going up one level
        requirements_file = os.path.join(parent_dir, 'requirements.txt')

        command = "pip3 install -r {}".format(requirements_file)
        result = subprocess.run(command, shell=True, text=True, capture_output=True)
        print(result.stdout)
    except Exception as e:
        print(f"An error occurred while installing packages: {e}")
        sys.exit(1)