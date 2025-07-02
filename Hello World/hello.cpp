#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>

int main() {
    const char* ascii_art = R"(
 ╔╗─╔╗╔═══╗╔╗───╔╗───╔═══╗
 ║║─║║║╔══╝║║───║║───║╔═╗║
 ║╚═╝║║╚══╗║║───║║───║║─║║
 ║╔═╗║║╔══╝║║─╔╗║║─╔╗║║─║║
 ║║─║║║╚══╗║╚═╝║║╚═╝║║╚═╝║
 ╚╝─╚╝╚═══╝╚═══╝╚═══╝╚═══╝
)";

    const char* profile = R"(
Name: akbar

Professional Profile
--------------------
- Electrical & Electronics Engineer
- 2 years in RF & high-speed PCB design
- Skilled with KiCad, Altium, SPICE, MATLAB, COMSOL
- RF circuits, amplifiers, filters, signal integrity
- EMI/EMC, PCB process, VNA & oscilloscope testing
)";

    const char* extra_ascii_art = R"(
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
)";

    while (true) {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        std::cout << ascii_art << std::endl;
        std::cout << profile << std::endl;
        std::cout << extra_ascii_art << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    return 0;
}