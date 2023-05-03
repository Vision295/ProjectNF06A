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
    
    def create_menu(self):
        """creates the menu (files, edit, selection, ...)"""
        self.menu = Menu(self)
        self.menu_file = Menu(self.menu, tearoff=0)
        self.menu_file.add_command(label="Open Folder", command=self.open_folder)
        self.menu.add_cascade(label="File", menu=self.menu_file)
        self.config(menu=self.menu)

    def open_folder(self):
        self.entries = os.listdir('C:/')
        self.labels = []
        for i in self.entries:
            self.labels.append(Button(self, text=i, font=("Verdana", 10, "italic"), fg='black',  justify=LEFT))
        for i, j in enumerate(self.labels):
            j.pack()



mainScreen = Window()
mainScreen.mainloop()

