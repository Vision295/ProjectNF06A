"""! @brief Example Python program with Doxygen style comments."""

##
# @mainpage Photostudio Documentation
#
# @section description_main Description
# PhotoStudio is a modern and innovative photo editing application that allows users to
# create stunning visual effects on their favorite photos. The app offers users a way to manage
# their photo gallery where they can easily upload and access their photos. It also offers a
# wide range of filters and editing options, users can transform their images into masterpieces
# with just a few clicks. The app also has a built-in compression feature, which reduces the
# file size of the resulting images without compromising their quality. This lightweight app
# is perfect for users who need to store or share their images without taking up too much
# storage space. Suppose all the photos are .png files.
#
# @section notes_main Notes
# Features : \n
#     - lossless compression using Huffman algorithm
#     - shows the list of available images
#     - explorer simulator
#     - resize images
#     - get metadata
#     - apply sepia, black and white, brightness, darkness and red filters
#     - apply custom rgb filters \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n
#
from explorer import Explorer

mainScreen = Explorer()
mainScreen.mainloop()
