from tkinter import *
from PIL import Image
from PIL import ImageTk

screen = Tk()
screen.geometry('500x500')
img = (Image.open("Project/Tkinter/test.png"))
img = img.resize((50, 50))
img = ImageTk.PhotoImage(img)


frame1 = Frame(borderwidth=1, relief='solid')

#scroll system
vertical_scrollBar = Scrollbar(frame1)
vertical_scrollBar.pack(side=RIGHT, fill=Y)



vertical_scrollBar.config()
frame1.pack(anchor=NW)

screen.mainloop()