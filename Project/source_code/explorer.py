import tkinter as ttk
import os
from image_manager import Image_manager

##
# @class         Explorer 
#                used to create a window which is a "windows explorer"
#                simulation through the computer to select images and go throw folders
# @param ttk.Tk  inherits from the Tk class
# 

class Explorer(ttk.Tk):

    ##
    # @brief         Constructor of Explorer 
    def __init__(self) -> None:
        """creates a window for the explorer and packs a scrollable 
        empty frame in it than open a directory to display it"""
        # initialize the tkinter super class
        ttk.Tk.__init__(self)
        # creates the menu (file, edit, selection, ...)
        self.create_menu()
        # size of the window
        self.geometry("800x500")
        self.labels = []
        self.folder_frame = ttk.Frame(self, bg='white')        

        # scrollable text area where all the buttons to select an image are
        self.area_file_buttons = ttk.Text(self.folder_frame) 
        self.scroll_bar = ttk.Scrollbar(self, command=self.area_file_buttons.yview)
        self.area_file_buttons.configure(yscrollcommand=self.scroll_bar.set, height=self.winfo_screenheight(), width=self.winfo_screenwidth())
       
        # .pack()
        self.area_file_buttons.pack()
        self.scroll_bar.pack(side=ttk.RIGHT, fill=ttk.Y)
        self.folder_frame.pack(fill=ttk.Y)

        # opens the folder 'gallery'
        self.current_path = os.getcwd()
        self.current_path = self.current_path.replace(chr(92), "/")  + "/../"
        self.current_path += 'gallery/'

        self.open()

    ##
    # @brief         empty the scrollable frame of all files when a new folder is openned
    def empty_folder_frame(self) -> None:
        """empty the scrollable frame of all files when a new folder is openned"""
        self.area_file_buttons.destroy()
        # scrollable text area where all the buttons to select an image are
        self.area_file_buttons = ttk.Text(self.folder_frame) 
        self.scroll_bar = ttk.Scrollbar(self, command=self.area_file_buttons.yview)
        self.area_file_buttons.configure(yscrollcommand=self.scroll_bar.set, height=self.winfo_screenheight(), width=self.winfo_screenwidth())
        # .pack()
        self.area_file_buttons.pack()
        self.scroll_bar.pack(side=ttk.RIGHT, fill=ttk.Y)
        self.folder_frame.pack(fill=ttk.Y)

    ## 
    # @brief         changes the path depending on what we open in the explorer
    # @param _path   is the path we add to the original self.current_path
    #                if it is a folder
    def change_path(self, _path:str) -> None:
        """changes the path depending on what we open in the explorer"""
        if os.path.isdir(os.path.abspath(self.current_path + _path + '/')):
            self.current_path += _path
            self.current_path += '/'
        # if it is an image
        elif _path.endswith(".png") or _path.endswith(".PNG"):
            self.current_path += _path
        self.open()

    ##
    # @brief        creates the menu (files, edit, selection, ...)
    def create_menu(self) -> None:
        """creates the menu (files, edit, selection, ...)"""
        self.menu = ttk.Menu(self)
        # menu button to open a folder
        self.menu_file = ttk.Menu(self.menu, tearoff=0)
        self.menu_file.add_command(label="Back", command=self.back)
        self.menu_file.add_command(label="Decompress", command=self.decompress)
        self.menu.add_cascade(label="Control", menu=self.menu_file)
        self.config(menu=self.menu)

    ##
    # @brief        creates a button based on the _text and _color which will represent a file in the frame self.frame of the explorer window
    # @param _test  is the name of the file (or directory)
    # @param _color is the color it is supposed to take based on its type"
    def create_button(self, _text:str, _color:str) -> None:
        """creates a button based on the _text and _color which will represent a file in the frame self.frame of the explorer window"""
        self.labels.append(ttk.Button(self.folder_frame, font=("Verdana", 10, "italic"), fg='black',  justify=ttk.LEFT, width=60, \
                                  bg=_color, text=_text, \
                                    command=lambda : self.change_path(_text)\
        ))

    ##
    # @brief         function run when a button in explorer is pressed to open a file or a folder :
    #        - if it is a png image it opens it 
    #        - if it is a file it doesn't do anything
    #        - if it is a folder it goes in the folder
    def open(self) -> None:
        """
        function run when a button in explorer is pressed to open a file or a folder :
            - if it is a png image it opens it 
            - if it is a file it doesn't do anything
            - if it is a folder it goes in the folder
        """
        self.open_directory()
        if self.current_path.endswith('.png') or self.current_path.endswith('.PNG'):
            self.image = Image_manager(self.back())

    ##
    # @brief          function run in the open function to open specifically a directory
    def open_directory(self) -> None:
        """function run in the open function to open specifically a directory"""
        if os.path.isdir(os.path.abspath(self.current_path)):
            self.empty_folder_frame()
            self.entries = os.listdir(self.current_path)
            # list of labels (list of all buttons which represent a file in path)
            self.labels.clear()
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
                self.area_file_buttons.window_create('end', window=self.labels[i])
                self.area_file_buttons.insert('end','\n')

    ##
    # @brief            function to go back in hierarchy
    # @return temp      where temp is the unmodified path
    def back(self) -> str:
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

    def decompress(self) -> None : os.system(os.getcwd().replace(chr(92), "/") + "/../executables/decoder.exe")