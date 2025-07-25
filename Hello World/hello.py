import time
import os

ascii_art = r"""
 ╔╗─╔╗╔═══╗╔╗───╔╗───╔═══╗
 ║║─║║║╔══╝║║───║║───║╔═╗║
 ║╚═╝║║╚══╗║║───║║───║║─║║
 ║╔═╗║║╔══╝║║─╔╗║║─╔╗║║─║║
 ║║─║║║╚══╗║╚═╝║║╚═╝║║╚═╝║
 ╚╝─╚╝╚═══╝╚═══╝╚═══╝╚═══╝
"""

profile = """
Name: akbar

Professional Profile
--------------------
- Electrical & Electronics Engineer
- 2 years in RF & high-speed PCB design
- Skilled with KiCad, Altium, SPICE, MATLAB, COMSOL
- RF circuits, amplifiers, filters, signal integrity
- EMI/EMC, PCB process, VNA & oscilloscope testing
"""

extra_ascii_art = r"""
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣠⠤⣤⣤⣤⡄⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⣭⣿⣾⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⣶⣶⣿⣿⣿⣿⡿⠿⠿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢾⣿⡿⠉⠁⠀⠀⠀⠀⠀⠈⢿⡇⢀⡀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣿⡇⢠⡴⢤⣤⡀⢴⣦⡤⣽⡇⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣇⠘⠛⣹⡿⣧⢈⡳⡉⢹⣿⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣶⠐⢫⢿⢟⡛⢳⠀⡾⠃⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⠉⣿⡄⠸⣿⢟⡿⣋⣿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⢹⡿⡄⠙⡋⢱⡖⣶⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣴⣿⣿⠺⣦⠾⢿⣏⣸⡇⡟⢷⣤⣀⡀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⢀⣠⣴⣾⣿⣿⣿⣿⣿⣇⠙⣧⡈⠁⣿⣷⣧⠸⣿⣿⣿⣿⣶⣤⣄⠀⠀
⠀⠀⠀⣴⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠈⠻⢦⣸⠏⠇⠀⣿⣿⣿⣿⣿⣿⣿⣷⠀
⠀⠀⣾⣿⣿⣿⣿⣿⣿⣿⡿⢻⣿⣿⣿⣷⣶⡒⠶⣷⠀⢠⣴⣿⣿⣿⣿⣿⣿⣿⣿⡆
⠠⠠⣿⣿⣿⣿⣿⣿⣿⡟⣶⡾⠋⠹⠿⣿⡿⠭⠭⠭⢤⣘⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇
⠀⠀⠸⣾⣿⣿⣿⣿⣿⠸⠏⠹⡐⠲⢶⣿⣯⠭⣉⣑⣲⣲⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇
⠀⠀⣰⣿⣿⣿⣿⣿⣿⣄⠀⣬⢁⣼⣿⣿⣿⡒⠒⡒⣿⢽⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇
⠀⢰⡏⢘⣿⣿⣿⡿⣻⡏⢰⠃⢸⣿⣿⣿⣿⡦⠭⢭⣽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇
⢠⣿⣿⡿⣿⣿⣿⣾⣿⡿⣾⣦⣿⣿⣿⣿⣿⣯⣭⣷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇
⢺⣿⣿⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣗⣺⣷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇
⢸⣿⣿⣿⣿⣿⣿⣷⣿⣿⣷⣿⣿⣿⣿⣿⣿⣷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠋⠀
⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣟⣻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡗⠀⠀
⠀⠹⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⣿⣿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠀
"""

while True:
    os.system('cls' if os.name == 'nt' else 'clear')
    print(ascii_art)
    print(profile)
    print(extra_ascii_art)
    time.sleep(2)
