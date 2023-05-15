import os

folder_path = 'C:/users/theop/Documents/'

png_files = [file for file in os.listdir(folder_path) if file.endswith('.png')]

for png_file in png_files:
    print(png_file)

