import os

folder_path = 'gallery'

png_files = [file for file in os.listdir(folder_path) if file.endswith('.png')]

for png_file in png_files:
    print(png_file)