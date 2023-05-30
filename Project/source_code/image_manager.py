##
# @brief imports : 
#       - tkinter : to create window + interactable interace
#       - PIL : to manage images
#       - hachoir : to extract metadatas from png images

import tkinter as ttk
import os
from PIL import Image
from PIL import ImageTk
from hachoir.parser import createParser
from hachoir.metadata import extractMetadata

##
# @class Image_manager 
# used to create a window which is opennend when the users clicks on 
# an image to preview it
# @param Tk inherits from the Toplevel class which is a child of Tk
# 

class Image_manager(ttk.Toplevel):
    ##
    # @brief Constructor of Image_manager 
    def __init__(self, _current_path:str) -> None:
        """creates a window to preview the image 
        @param _current_path is to get the path in order to go back in hierarchy 
        when image is closed"""
        # initialize the tkinter super class
        ttk.Toplevel.__init__(self)

        # set default settings
        self.current_path = _current_path
        self.window_size = 400
        self.resizable(False, False)

        # image parameters
        self.image = Image.open(os.path.abspath(self.current_path))
        self.image_width, self.image_height = self.image.size
        self.modified_image_size = [0, 0]

        self.open_image_png()

    def open_image_png(self) -> None:
        """function run when the image window is created to load an image on it"""
        self.resize_image_to_fit()

        # load on screen
        self.image_load = ImageTk.PhotoImage(self.modified_image_to_fit)
        self.label_image = ttk.Label(self, image=self.image_load) 
        self.label_image.pack()

        self.create_menu()
        self.mainloop()

    def resize_image_to_fit(self) -> None:
        """function run in the open_image_png function to make it fit within the size of the window"""

        # reduce the size of the image for it to fit within the size of the the window and fill it as much as possible
        if self.image_width > self.image_height:
            self.modified_image_size = (self.window_size, int(self.image_height * self.window_size / self.image_width))
        else: 
            self.modified_image_size = (self.window_size, int(self.image_height * self.window_size / self.image_width))
        # create a new image with resized dimensions
        self.modified_image_to_fit = self.image.resize(self.modified_image_size)

    def create_menu(self) -> None:
        """function run in the open_image_png function to create a menu"""

        self.image_menu = ttk.Menu(self)
        # menu button to edit images
        self.image_menu_edit = ttk.Menu(self.image_menu, tearoff=0)
        self.image_menu_info = ttk.Menu(self.image_menu, tearoff=0)

        # resize
        self.image_menu_edit.add_command(label="Resize", command=self.resize_window)
        # apply filters
        self.image_menu_edit.add_command(label="Custom filter", command=self.custom_filters_window)
        self.image_menu_edit.add_command(label="Predefined filter", command=self.predefined_filters_window)
        # compress
        self.image_menu_edit.add_command(label="Compress", command=self.compress)
        self.image_menu_edit.add_command(label="Decompress", command=self.decompress)
        # option to edit image
        self.image_menu.add_cascade(label="Edit", menu=self.image_menu_edit)
        # option to show metadata
        self.image_menu_info.add_command(label="Metadata", command=self.metadata)
        # options to show info
        self.image_menu.add_cascade(label="Info", menu=self.image_menu_info)

        self.config(menu=self.image_menu)

    def resize_window(self) -> None:
        """function used to resize the image currently openned by openning a new window to select the size"""

        # creation of the window to rescale the image
        self.edit_image_window_size = [100, 100]
        self.edit_image_window = ttk.Toplevel(self, height=self.edit_image_window_size[0], width=self.edit_image_window_size[1])
        self.edit_image_window.resizable(width=False, height=False)
        
        # labels in front of the entries inside the parametrization window
        ttk.Label(self.edit_image_window, text="Width ").grid(row=0)
        ttk.Label(self.edit_image_window, text="Height ").grid(row=1)
        
        # list of the different entries
        self.image_size_entries = [
            # set defaults values at the size of the image without rescaling it
            ttk.Entry(self.edit_image_window, textvariable=ttk.StringVar(self.edit_image_window, str(self.image_width))),
            ttk.Entry(self.edit_image_window, textvariable=ttk.StringVar(self.edit_image_window, str(self.image_height)))
        ]

        # printing the entries on window
        self.image_size_entries[0].grid(row=0, column=1)
        self.image_size_entries[1].grid(row=1, column=1)

        # input of the user to resize the image
        self.input_image_size = [0, 0]

        # button to validate the new dimensions of the image
        self.resize_button = ttk.Button(self.edit_image_window, text="Resize", command=self.change_size_values)
        self.resize_button.grid(row=2, column=0)
        # update edit_image_window
        self.edit_image_window.mainloop()

    def change_size_values(self) -> None:
        """function to change the scale of the image and saves the modified image as the name of the image + resized.png"""

        # get the input of the user
        self.input_image_size = (int(self.image_size_entries[0].get()), int(self.image_size_entries[1].get()))
        # open, resize and save the image
        self.new_path = self.current_path[:-4] + "resized" + self.current_path[-4:]
        Image.open(os.path.abspath(self.current_path)).resize(self.input_image_size).save(self.new_path)
        self.current_path = self.new_path

        # reload the image
        self.edit_image_window.destroy()
        self.destroy()

    def metadata(self) -> None:
        """function run to display on screen the metadata of a png image"""

        # window creation
        self.metadata_image_window = ttk.Toplevel(self)
        self.metadata_image_window.resizable(False, False)

        self.metadata_text_zone = ttk.Text(self.metadata_image_window, font=("Comic Sans MS", 20, "bold"), height=10, width=30)
        # get metadata but unreadable
        self.exifdata = self.image.getexif()
        # convert metadata into chain of character to display it
        self.metadata_text = ""
        parser = createParser(os.path.abspath(self.current_path))
        metadata = extractMetadata(parser)
        # here my IDE doesn't detect exportPlaintext() as a correct method whereas it works
        for line in metadata.exportPlaintext() : self.metadata_text += line + "\n"

        # display metadata on screen
        self.metadata_text_zone.insert(ttk.INSERT, self.metadata_text)
        self.metadata_text_zone.grid(row=0, column=0)

    def get_new_pixel(self, _old_px:list, _mask:list) -> tuple:
        """function used to change the value rgb of a pixel by applying a filter
        @param _old_px is the rgb values of the pixel
        @param _mask is the filter to apply to each pixel
        @return is a tuple which corresponds to a modified pixel"""
        return (
            int(_old_px[0] * _mask[0][0] + _old_px[1] * _mask[0][1] + _old_px[2] * _mask[0][2]),
            int(_old_px[0] * _mask[1][0] + _old_px[1] * _mask[1][1] + _old_px[2] * _mask[1][2]),
            int(_old_px[0] * _mask[2][0] + _old_px[1] * _mask[2][1] + _old_px[2] * _mask[2][2])
        )
    
    def apply_predefined_filter(self) -> None:
        # intensity of the filter (could be modified later on to add another fonctionnality)
        self.intensity = 1/3
        # Define the filter weights for different predefined filters
        self.predefined_filters_choice = {
            "sepia": ((0.393, 0.769, 0.189), (0.349, 0.686, 0.168), (0.272, 0.534, 0.131)),
            "black_and_white": ((0, 0, 0), (0, 0, 0), (0, 0, 0)),
            "brightness": ((1, self.intensity, self.intensity), (self.intensity, 1, self.intensity), (self.intensity, self.intensity, 1)),
            "darkness": ((1 - self.intensity, 0.0, 0.0), (0.0, 1 - self.intensity, 0.0), (0.0, 0.0, 1 - self.intensity)),
            "red_filter": ((1.0, 0.0, 0.0), (self.intensity, 1.0, 0.0), (self.intensity, 0.0, 1.0)),
            "green_filter": ((1.0, self.intensity, 0.0), (0.0, 1.0, 0.0), (0.0, self.intensity, 1.0)), 
            "blue_filter": ((1.0, 0.0, self.intensity), (0.0, 1.0, self.intensity), (0.0, 0.0, 1.0))
            # Add more filters here if needed
        }
        if self.selected_filter != "black_and_white":
            self.apply_custom_filter(_mask=self.predefined_filters_choice[self.selected_filter])
        # specific case for blach and white filter which is applied a different way
        else: 
            self.filtered_image = self.image.convert("L")
            # Save the modified image with a modified name
            modified_name = self.current_path[:-4] + "filtered" + self.current_path[-4:]  # Add "_filtered" before the file extension
            self.filtered_image.save(modified_name)

            # reload the image
            self.edit_image_window.destroy()
            self.destroy()

    def apply_custom_filter(self, _mask:tuple) -> None:
        """function run when the button "filter" is pressed to apply a custom
        filter
        @param _mask is the filter to apply to each pixel"""
        # we copy the image
        self.filtered_image = self.image.copy()
        # loop through the pixels of the image
        for y in range(0, self.image_height - 1):
            for x in range(0, self.image_width - 1):
                # modify every pixel and save them in the copy of the original image
                px = self.image.getpixel((x, y))
                new_px = self.get_new_pixel(px, _mask)
                self.filtered_image.putpixel((x, y), new_px)
        
        # Save the modified image with a modified name
        modified_name = self.current_path[:-4] + "filtered" + self.current_path[-4:]  # Add "_filtered" before the file extension
        self.filtered_image.save(modified_name)

        # reload the image
        self.edit_image_window.destroy()
        self.destroy()

    def custom_filters_window(self) -> None:
        """function used to apply a custom filter to the image currently openned by openning a new window to select the size"""
        # creation of the window to rescale the image
        self.edit_image_window_size = [100, 100]
        self.edit_image_window = ttk.Toplevel(self, height=self.edit_image_window_size[0], width=self.edit_image_window_size[1])
        self.edit_image_window.resizable(width=False, height=False)
        
        # labels in front of the entries inside the parametrization window
        ttk.Label(self.edit_image_window, text="Enter three rgb coefficient between 0 and 1 for each color").grid(row=0, column=0)
        ttk.Label(self.edit_image_window, text="R ").grid(row=1, column=0)
        ttk.Label(self.edit_image_window, text="G ").grid(row=2, column=0)
        ttk.Label(self.edit_image_window, text="B ").grid(row=3, column=0)

        # list of the different entries
        self.image_filter_entries = [[ttk.Entry(self.edit_image_window, textvariable=ttk.StringVar(self.edit_image_window)) for i in range(3)] for j in range(3)]
        
        # loop through fields to display all entries
        for indexI, i in enumerate(self.image_filter_entries):
            for indexJ, j in enumerate(i):
                # printing the entries on window
                j.grid(row=indexJ + 1, column=indexI + 1)

        # button to validate the new dimensions of the image
        self.custom_filter_button = ttk.Button(self.edit_image_window, text="Filter", command=self.get_custom_filter)
        self.custom_filter_button.grid(row=4, column=0)
        # update edit_image_window
        self.edit_image_window.mainloop()

    def predefined_filters_window(self) -> None:
        """function used to apply a custom filter to the image currently openned by openning a new window to select the size"""
        # creation of the window to rescale the image
        self.edit_image_window_size = [100, 100]
        self.edit_image_window = ttk.Toplevel(self, height=self.edit_image_window_size[0], width=self.edit_image_window_size[1])
        self.edit_image_window.resizable(width=False, height=False)

        # labels in front of the entries inside the parametrization window
        ttk.Label(self.edit_image_window, text="Select a filter of your choice between the following : ").grid(row=0, column=0)
        self.options = [
            "sepia",
            "black_and_white",
            "brightness",
            "darkness",
            "red_filter",
            "green_filter",
            "blue_filter",
        ]
        # represents the input of the user
        self.choice = ttk.StringVar(self.edit_image_window)
        # default value
        self.choice.set("sepia")
        # create a selection field
        self.option_menu = ttk.OptionMenu(self.edit_image_window, self.choice, *self.options)
        # displays it
        self.option_menu.grid(row=1, column=0)

        # set a button to validate the choice
        self.predefined_filter_button = ttk.Button(self.edit_image_window, text="Filter", command=self.get_predefined_filter)
        self.predefined_filter_button.grid(row=2, column=0)

        # update edit_image_window
        self.edit_image_window.mainloop()

    def get_predefined_filter(self) -> None : self.selected_filter = self.choice.get(); self.apply_predefined_filter()

    def get_custom_filter(self) -> None: 
        """function to store the values of enterred by the user to create a custom
        filter in self.filter_values"""
        # empty matrix
        self.filter_values = [[0.0 for i in range(3)] for j in range(3)]
        # loop through entries to get values
        for indexI, i in enumerate(self.filter_values):
            for indexJ, _ in enumerate(i):
                self.filter_values[indexI][indexJ] = float(self.image_filter_entries[indexI][indexJ].get())
        # convert to tuple
        self.filter_values = tuple(tuple(i) for i in self.filter_values)
        self.apply_custom_filter(_mask=self.filter_values)

    def compress(self) -> None:
        """function to compress an openned image and save the compress version in a folder"""
        # Create the folder if it doesn't exist
        tempList_current_path = list(self.current_path)

        # pop the first element to loop through
        filename =  tempList_current_path.pop()
        while filename[-1] != '/' : filename += tempList_current_path.pop()
        filename = filename.replace('/', '')
        filename = filename[::-1]
        folder_name = "compressed"

        self.current_path = ''
        for i in tempList_current_path : self.current_path += i
        # add a slash for it to be readable
        self.current_path += '/'
        self.current_path = self.current_path.replace("gallery/", "")

        # Create the text file and write the file name into it
        text_file_path = self.current_path + folder_name + "/file_to_compress.txt"
        
        file = open(text_file_path, "w")
        file.write(filename)
        file.close()
        # verif ça
        os.system(os.getcwd().replace(chr(92), "/") + "/../executables/encoder.exe")

    def decompress(self) -> None : os.system(os.getcwd().replace(chr(92), "/") + "/../executables/decoder.exe")