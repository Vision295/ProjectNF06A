from tkinter import *
import os
from PIL import Image
from PIL import ImageTk
from pathlib import Path


image = Image.open(os.path.abspath("Project/Tkinter/test.png"))
image.resize((10, 100))
image.save("test2.png")