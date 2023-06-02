# **Photo Studio** Project
NF06 P23 Project by:

-*Théophile Wachtel*

-*Lucas Schummer*


---


As this project doesn't have yet executable files to launch the interface, you should double-click on "main.py" to start the program.
Make sure the necessary python libraries are installed in your environment.



---



Includes:

`pip install PIL`

`pip install tkinter`

`pip install hachoir`



---



*Note :* You can manually use the Huffman Coding Algorithm by placing any file in the "gallery" folder, writing its name in encoded/file_to_encode.txt and launching "executables/encoder.exe"


In the compressed folder, you will find the following files, necessary for the decompression :

-"encoded.bin" : the file encoded thanks to the Huffman Coding

-"conversion.bin" : the file containing the necessary information so that the decoder can rebuild the Huffman tree and decompress the encoded file

-"compressed.txt : the name of the file which is currently compressed


You can then decompress the encoded file launching "executables/decoder.exe" (Make sure the files listed above are in the folder compressed)
