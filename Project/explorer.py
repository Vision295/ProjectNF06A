from tkinter import *
import os
from image_manager import Image_manager


class Explorer(Tk):

    def __init__(self) -> None:
        # initialize the tkinter super class
        Tk.__init__(self)
        # creates the menu (file, edit, selection, ...)
        self.create_menu()
        # size of the window
        self.geometry("800x500")
        self.lables = []
        self.folder_frame = Frame(self, bg='white')
        # CHANGER CA WESH
        self.current_path = 'C:/users/theop/Documents/_Perso/'

        # scrollable text area where all the buttons to select an image are
        self.area_file_buttons = Text(self.folder_frame) 
        self.scroll_bar = Scrollbar(self, command=self.area_file_buttons.yview)
        self.area_file_buttons.configure(yscrollcommand=self.scroll_bar.set, height=self.winfo_screenheight(), width=self.winfo_screenwidth())
       
        # .pack()
        self.area_file_buttons.pack()
        self.scroll_bar.pack(side=RIGHT, fill=Y)
        self.folder_frame.pack(fill=Y)

        self.open()
    
    def empty_folder_frame(self):
        """empty the scrollable frame of all files when a new folder is openned"""
        self.area_file_buttons.destroy()
        # scrollable text area where all the buttons to select an image are
        self.area_file_buttons = Text(self.folder_frame) 
        self.scroll_bar = Scrollbar(self, command=self.area_file_buttons.yview)
        self.area_file_buttons.configure(yscrollcommand=self.scroll_bar.set, height=self.winfo_screenheight(), width=self.winfo_screenwidth())
        # .pack()
        self.area_file_buttons.pack()
        self.scroll_bar.pack(side=RIGHT, fill=Y)
        self.folder_frame.pack(fill=Y)

    def change_path(self, _path):
        """changes the self.current_path to self.curr_path +  _path if it is a directory and opens the image if it is a png"""
        if os.path.isdir(os.path.abspath(self.current_path + _path + '/')):
            self.current_path += _path
            self.current_path += '/'
        elif _path.endswith(".png") or _path.endswith(".PNG"):
            self.current_path += _path
        self.open()
    
    def create_menu(self):
        """creates the menu (files, edit, selection, ...)"""
        self.menu = Menu(self)
        # menu button to open a folder
        self.menu_file = Menu(self.menu, tearoff=0)
        self.menu_file.add_command(label="Back", command=self.back)
        self.menu.add_cascade(label="Control", menu=self.menu_file)
        self.config(menu=self.menu)

    def create_button(self, _text, _color):
        """creates a button based on the _text and _color where _text is the name of the file (or directory)
        and _ color is the color it is supposed to take based on its type"""
        self.lables.append(Button(self.folder_frame, font=("Verdana", 10, "italic"), fg='black',  justify=LEFT, width=60, \
                                  bg=_color, text=_text, \
                                    command=lambda : self.change_path(_text)\
        ))

    def open(self):
        """
        function run when a button in explorer is pressed to open a file or a folder :
            - if it is a png image it opens it 
            - if it is a file it doesn't do anything
            - if it is a folder it goes in the folder
        """
        self.open_directory()
        if self.current_path.endswith('.png') or self.current_path.endswith('.PNG'):
            self.image = Image_manager(self.back())
            
    def open_directory(self):
        """function run in the open function to open specifically a directory"""
        if os.path.isdir(os.path.abspath(self.current_path)):
            self.empty_folder_frame()
            self.entries = os.listdir(self.current_path)
            # list of labels (list of all buttons which represent a file in path)
            self.lables.clear()
            for i, j in enumerate(self.entries):

                # if it is a png image
                if j.endswith('.png') or j.endswith('.PNG'):
                    self.create_button(j, 'green')
                # if it is a file but not png
                elif os.path.isfile(os.path.abspath(self.current_path + j + '/')):
                    self.create_button(j, 'white')
                # if it is a directory
                elif os.path.isdir(os.path.abspath(self.current_path + j + '/')):
                    self.create_button(j, 'yellow')

                # displays the buttons in scrollable window
                self.area_file_buttons.window_create('end', window=self.lables[i])
                self.area_file_buttons.insert('end','\n')

    def back(self):
        """function to go back in hierarchy"""
        # conversion str -> list to manipulate it easier
        temp = self.current_path
        tempList_current_path = list(self.current_path)
        # pop the first two elements so that a is different from '/'
        tempValue_tempList = tempList_current_path.pop()
        tempValue_tempList = tempList_current_path.pop()

        while tempValue_tempList != '/' : tempValue_tempList = tempList_current_path.pop()
        
        # conversion lit -> str to get current_path reusable
        self.current_path = ''
        for i in tempList_current_path : self.current_path += i
        # add a slash for it to be readable
        self.current_path += '/'
        self.open()
        return temp

