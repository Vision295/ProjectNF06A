from tkinter import *
import os
from PIL import Image
from PIL import ImageTk
from pathlib import Path
from hachoir.parser import createParser
from hachoir.metadata import extractMetadata

class Image_manager(Toplevel):

    def __init__(self, _current_path) -> None:
        Toplevel.__init__(self)
        self.current_path = _current_path
        self.window_size = 500
        self.resizable(False, False)

        self.image = Image.open(os.path.abspath(self.current_path))
        self.image_width, self.image_height = self.image.size
        self.modified_image_size = [0, 0]

        self.open_image_png()

    def open_image_png(self):
        """function run when the image window is created to load an image on it"""

        # RESIZE IT
        self.resize_image_to_fit()
        
        # LOAD ON SCREEN
        self.image_load = ImageTk.PhotoImage(self.modified_image_to_fit)
        self.label_image = Label(self, image=self.image_load) 
        self.label_image.pack()

        # MENU
        self.create_menu()
        # MANLOOP
        self.mainloop()

    def resize_image_to_fit(self):
        """function run in the open_image_png function to make it fit within the size of the window"""

        # reduce the size of the image for it to fit within the size of the the window and fill it as much as possible
        if self.image_width > self.image_height:
            self.modified_image_size[0] = self.window_size
            self.modified_image_size[1] = int(self.image_height * self.window_size / self.image_width)
        else: 
            self.modified_image_size[1] = self.window_size
            self.modified_image_size[0] = int(self.image_height * self.window_size / self.image_width)
        # create a new image with resized dimensions
        self.modified_image_to_fit = self.image.resize(self.modified_image_size)

    def create_menu(self):
        """function run in the open_image_png function to create a menu"""

        self.image_menu = Menu(self)
        # menu button to edit images
        self.image_menu_edit = Menu(self.image_menu, tearoff=0)
        self.image_menu_info = Menu(self.image_menu, tearoff=0)

        # resize
        self.image_menu_edit.add_command(label="Resize", command=self.resize)
        # apply filter
        self.image_menu_edit.add_command(label="Custom filter", command=self.custom_filters_window)
        self.image_menu_edit.add_command(label="Predefined filter", command=self.predefined_filters_window)
        # option to edit image
        self.image_menu.add_cascade(label="Edit", menu=self.image_menu_edit)
        # option to show metadata
        self.image_menu_info.add_command(label="Metadata", command=self.metadata)
        # options to show info
        self.image_menu.add_cascade(label="Info", menu=self.image_menu_info)
        self.config(menu=self.image_menu)

    def resize(self):
        """function used to resize the image currently openned by openning a new window to select the size"""

        # creation of the window to rescale the image
        self.edit_image_window_size = [100, 100]
        self.edit_image_window = Toplevel(self, height=self.edit_image_window_size[0], width=self.edit_image_window_size[1])
        self.edit_image_window.resizable(width=False, height=False)
        
        # labels in front of the entries inside the parametrization window
        Label(self.edit_image_window, text="Width ").grid(row=0)
        Label(self.edit_image_window, text="Height ").grid(row=1)
        
        # list of the different entries
        self.image_size_entries = [
            # set defaults values at the size of the image without rescaling it
            Entry(self.edit_image_window, textvariable=StringVar(self.edit_image_window, str(self.image_width))),
            Entry(self.edit_image_window, textvariable=StringVar(self.edit_image_window, str(self.image_height)))
        ]

        # printing the entries on window
        self.image_size_entries[0].grid(row=0, column=1)
        self.image_size_entries[1].grid(row=1, column=1)

        # input of the user to resize the image
        self.input_image_size = [0, 0]

        # button to validate the new dimensions of the image
        self.resize_button = Button(self.edit_image_window, text="Resize", command=self.change_size_values)
        self.resize_button.grid(row=2, column=0)
        # update edit_image_window
        self.edit_image_window.mainloop()

    def change_size_values(self):
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

    def metadata(self):
        """function run to display on screen the metadata of a png image"""

        # window creation
        self.metadata_image_window = Toplevel(self)
        self.metadata_image_window.resizable(False, False)

        self.metadata_text_zone = Text(self.metadata_image_window, font=("Comic Sans MS", 20, "bold"), height=10, width=30)
        # get metadata but unreadable
        self.exifdata = self.image.getexif()
        # convert metadata into chain of character to display it
        self.metadata_text = ""
        parser = createParser(os.path.abspath(self.current_path))
        metadata = extractMetadata(parser)
        for line in metadata.exportPlaintext() : self.metadata_text += line + "\n"

        # display metadata on screen
        self.metadata_text_zone.insert(INSERT, self.metadata_text)
        self.metadata_text_zone.grid(row=0, column=0)

    def get_new_pixel(self, _old_px, _mask):
        return (
            int(_old_px[0] * _mask[0][0] + _old_px[1] * _mask[0][1] + _old_px[2] * _mask[0][2]),
            int(_old_px[0] * _mask[1][0] + _old_px[1] * _mask[1][1] + _old_px[2] * _mask[1][2]),
            int(_old_px[0] * _mask[2][0] + _old_px[1] * _mask[2][1] + _old_px[2] * _mask[2][2])
        )
    
    def apply_predefined_filter(self):

        self.intensity = 1/3
        # Define the filter weights for different filters
        self.predefined_filters_choice = {
            "sepia": ((0.393, 0.769, 0.189), (0.349, 0.686, 0.168), (0.272, 0.534, 0.131)),
            "black_and_white": ((0.2989, 0.5870, 0.1140),),
            "brightness": ((1, self.intensity, self.intensity), (self.intensity, 1, self.intensity), (self.intensity, self.intensity, 1)),
            "darkness": ((1 - self.intensity, 0.0, 0.0), (0.0, 1 - self.intensity, 0.0), (0.0, 0.0, 1 - self.intensity)),
            "red_filter": ((1.0, 0.0, 0.0), (self.intensity, 1.0, 0.0), (self.intensity, 0.0, 1.0)),
            "green_filter": ((1.0, self.intensity, 0.0), (0.0, 1.0, 0.0), (0.0, self.intensity, 1.0)), 
            "blue_filter": ((1.0, 0.0, self.intensity), (0.0, 1.0, self.intensity), (0.0, 0.0, 1.0))
            # Add more filters here if needed
        }
        
        self.apply_custom_filter(_mask=self.predefined_filters_choice[self.selected_filter])
 
    def apply_custom_filter(self, _mask=None):
        self.filtered_image = self.image.copy()
        for y in range(0, self.image_height - 1):
            for x in range(0, self.image_width - 1):
                px = self.image.getpixel((x, y))
                new_px = self.get_new_pixel(px, _mask)
                self.filtered_image.putpixel((x, y), new_px)
        
        # Save the modified image with a modified name
        modified_name = self.current_path[:-4] + "filtered" + self.current_path[-4:]  # Add "_filtered" before the file extension
        self.filtered_image.save(modified_name)

        # reload the image
        self.edit_image_window.destroy()
        self.destroy()

    def custom_filters_window(self):
        """function used to apply a custom filter to the image currently openned by openning a new window to select the size"""
        # creation of the window to rescale the image
        self.edit_image_window_size = [100, 100]
        self.edit_image_window = Toplevel(self, height=self.edit_image_window_size[0], width=self.edit_image_window_size[1])
        self.edit_image_window.resizable(width=False, height=False)
        
        # labels in front of the entries inside the parametrization window
        Label(self.edit_image_window, text="Enter three rgb coefficient between 0 and 1 for each color").grid(row=0, column=0)
        Label(self.edit_image_window, text="R ").grid(row=1, column=0)
        Label(self.edit_image_window, text="G ").grid(row=2, column=0)
        Label(self.edit_image_window, text="B ").grid(row=3, column=0)

        # list of the different entries
        self.image_filter_entries = [[Entry(self.edit_image_window, textvariable=StringVar(self.edit_image_window, 0)) for i in range(3)] for j in range(3)]
        
        for indexI, i in enumerate(self.image_filter_entries):
            for indexJ, j in enumerate(i):
                # printing the entries on window
                j.grid(row=indexJ + 1, column=indexI + 1)

        # button to validate the new dimensions of the image
        self.custom_filter_button = Button(self.edit_image_window, text="Filter", command=self.apply_filter)
        self.custom_filter_button.grid(row=4, column=0)
        # update edit_image_window
        self.edit_image_window.mainloop()

    def predefined_filters_window(self):
        """function used to apply a custom filter to the image currently openned by openning a new window to select the size"""
        # creation of the window to rescale the image
        self.edit_image_window_size = [100, 100]
        self.edit_image_window = Toplevel(self, height=self.edit_image_window_size[0], width=self.edit_image_window_size[1])
        self.edit_image_window.resizable(width=False, height=False)

        # labels in front of the entries inside the parametrization window
        Label(self.edit_image_window, text="Select a filter of your choice between the following : ").grid(row=0, column=0)
        self.options = [
            "sepia",
            "black_and_white,"
            "brightness",
            "darkness",
            "red_filter",
            "green_filter",
            "blue_filter",
        ]
        self.choice = StringVar(self.edit_image_window)
        self.choice.set("select an option")
        self.option_menu = OptionMenu(self.edit_image_window, self.choice, *self.options)
        self.option_menu.grid(row=1, column=0)

        self.predefined_filter_button = Button(self.edit_image_window, text="Filter", command=self.get_predefined_filter)
        self.predefined_filter_button.grid(row=2, column=0)

        # update edit_image_window
        self.edit_image_window.mainloop()

    def get_predefined_filter(self) : self.selected_filter = self.choice.get(); self.apply_predefined_filter()

    def get_custom_filter(self): 
        self.filter_values = [[0 for i in range(3)] for j in range(3)]
        for indexI, i in enumerate(self.filter_values):
            for indexJ, _ in enumerate(i):
                self.filter_values[indexI][indexJ] = float(self.image_filter_entries[indexI][indexJ].get())

        self.filter_values = tuple(tuple(i) for i in self.filter_values)
        self.apply_custom_filter(_mask=self.filter_values)