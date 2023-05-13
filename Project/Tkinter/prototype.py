from tkinter import *
import os


class Window(Tk):

    def __init__(self) -> None:
        # initialize the tkinter super class
        Tk.__init__(self)
        # creates the menu (file, edit, selection, ...)
        self.create_menu()
        # size of the window
        self.geometry("800x500")
        # 
        self.folder_frame = Frame(self, bg='white')
        # scroll system
        self.scroll_bar = Scrollbar(self)
        
        # scrollable text area where all the buttons to select an image are
        self.area_file_buttons = Text(self.folder_frame)
        self.area_file_buttons.configure(yscrollcommand=self.scroll_bar.set, height=self.winfo_screenheight(), width=self.winfo_screenwidth())
        self.scroll_bar.configure(command=self.area_file_buttons.yview)

        # .pack()
        self.area_file_buttons.pack()
        self.scroll_bar.pack(side=RIGHT, fill=Y)
        self.folder_frame.pack(fill=Y)
    
    def create_menu(self):
        """creates the menu (files, edit, selection, ...)"""
        self.menu = Menu(self)
        self.menu_file = Menu(self.menu, tearoff=0)
        self.menu_file.add_command(label="Open Folder", command=self.open_folder)
        self.menu.add_cascade(label="File", menu=self.menu_file)
        self.config(menu=self.menu)

    def open_folder(self):
        self.entries = os.listdir('C:/User/')
        self.labels = []
        for i, j in enumerate(self.entries):
            self.labels.append(Button(self.folder_frame, text=j, font=("Verdana", 10, "italic"), fg='black',  justify=LEFT))
            self.area_file_buttons.window_create('end', window=self.labels[i])
            self.area_file_buttons.insert('end','\n')



mainScreen = Window()
mainScreen.mainloop()

