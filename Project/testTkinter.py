from tkinter import *

screen = Tk()
screen.geometry('500x500')
screen.title('Project NF06A')
screen['bg'] = 'green'
screen.resizable(height=False, width=False)

label = Label(screen, text="testing tkinter", font=("Verdana", 20, "italic"), fg='white', bg='black')
label["text"] = "something new"
label.pack(padx=50)

def testingIt():
    print("You've clicked on the button")

button = Button(screen, text="testing it", bg='yellow', fg='black', command=testingIt)

def enter():
    display['text'] = entry.get()

# define a string before using it
strVar = StringVar()
display = Label(screen, text="random stuff")
display.pack()
entry = Entry(screen, textvariable=strVar)
entry.pack()
enterInfo = Button(screen, text="enter the text you want to enter", command=enter)
enterInfo.pack()
screen.mainloop()