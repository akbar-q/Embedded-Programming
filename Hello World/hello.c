#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

const char* ascii_art =
" ╔╗─╔╗╔═══╗╔╗───╔╗───╔═══╗\n"
" ║║─║║║╔══╝║║───║║───║╔═╗║\n"
" ║╚═╝║║╚══╗║║───║║───║║─║║\n"
" ║╔═╗║║╔══╝║║─╔╗║║─╔╗║║─║║\n"
" ║║─║║║╚══╗║╚═╝║║╚═╝║║╚═╝║\n"
" ╚╝─╚╝╚═══╝╚═══╝╚═══╝╚═══╝\n";

const char* profile =
"Name: akbar\n"
"\n"
"Professional Profile\n"
"--------------------\n"
"- Electrical & Electronics Engineer\n"
"- 2 years in RF & high-speed PCB design\n"
"- Skilled with KiCad, Altium, SPICE, MATLAB, COMSOL\n"
"- RF circuits, amplifiers, filters, signal integrity\n"
"- EMI/EMC, PCB process, VNA & oscilloscope testing\n";

const char* extra_ascii_art =
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣠⠤⣤⣤⣤⡄⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⣭⣿⣾⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⣶⣶⣿⣿⣿⣿⡿⠿⠿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢾⣿⡿⠉⠁⠀⠀⠀⠀⠀⠈⢿⡇⢀⡀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣿⡇⢠⡴⢤⣤⡀⢴⣦⡤⣽⡇⠀⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣇⠘⠛⣹⡿⣧⢈⡳⡉⢹⣿⠀⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣶⠐⢫⢿⢟⡛⢳⠀⡾⠃⠀⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⠉⣿⡄⠸⣿⢟⡿⣋⣿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⢹⡿⡄⠙⡋⢱⡖⣶⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣴⣿⣿⠺⣦⠾⢿⣏⣸⡇⡟⢷⣤⣀⡀⠀⠀⠀⠀⠀⠀\n"
"⠀⠀⠀⠀⠀⢀⣠⣴⣾⣿⣿⣿⣿⣿⣇⠙⣧⡈⠁⣿⣷⣧⠸⣿⣿⣿⣿⣶⣤⣄⠀⠀\n"
"⠀⠀⠀⣴⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠈⠻⢦⣸⠏⠇⠀⣿⣿⣿⣿⣿⣿⣿⣷⠀\n"
"⠀⠀⣾⣿⣿⣿⣿⣿⣿⣿⡿⢻⣿⣿⣿⣷⣶⡒⠶⣷⠀⢠⣴⣿⣿⣿⣿⣿⣿⣿⣿⡆\n"
"⠠⠠⣿⣿⣿⣿⣿⣿⣿⡟⣶⡾⠋⠹⠿⣿⡿⠭⠭⠭⢤⣘⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇\n"
"⠀⠀⠸⣾⣿⣿⣿⣿⣿⠸⠏⠹⡐⠲⢶⣿⣯⠭⣉⣑⣲⣲⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇\n"
"⠀⠀⣰⣿⣿⣿⣿⣿⣿⣄⠀⣬⢁⣼⣿⣿⣿⡒⠒⡒⣿⢽⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇\n"
"⠀⢰⡏⢘⣿⣿⣿⡿⣻⡏⢰⠃⢸⣿⣿⣿⣿⡦⠭⢭⣽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇\n"
"⢠⣿⣿⡿⣿⣿⣿⣾⣿⡿⣾⣦⣿⣿⣿⣿⣿⣯⣭⣷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇\n"
"⢺⣿⣿⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣗⣺⣷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇\n"
"⢸⣿⣿⣿⣿⣿⣿⣷⣿⣿⣷⣿⣿⣿⣿⣿⣿⣷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠋⠀\n"
"⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣟⣻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡗⠀⠀\n"
"⠀⠹⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⣿⣿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠀\n";

int main() {
    while (1) {
#ifdef _WIN32
        system("cls");
        Sleep(2000);
#else
        system("clear");
        usleep(2000000);
#endif
        printf("%s\n", ascii_art);
        printf("%s\n", profile);
        printf("%s\n", extra_ascii_art);
    }
    return