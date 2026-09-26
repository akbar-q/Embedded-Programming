# Smart Shower Water Conservation System: Presentation Script

Hello, my project is called the Smart Shower Water Conservation System.

The problem is that people can waste a lot of water and energy by taking long showers or leaving the water running while shampooing or soaping.

My project is a foam-board prototype which demonstrates how a smart shower could help. It is controlled by an ESP32 WROOM-32D microcontroller. Two potentiometers represent the hot and cold taps, so the controller can measure the user's chosen demand and hot-water use.

The ESP32 runs a decision-making algorithm throughout the shower. It reads the controls, tracks time, calculates a conservation score, and compares the results with configurable water and energy budgets. This means the system responds to how the shower is being used, instead of only using a basic timer.

The WS2812 LED ring is the visual interface. Green shows efficient use, amber and orange give early warnings, blue reminds the user to pause while shampooing or soaping, and red shows that the conservation limit has been reached.

If excessive use continues, a servo motor gradually demonstrates a flow restriction. The system does not suddenly stop the shower. It first gives feedback, allows the user time to change their behaviour, and then applies restriction in small stages. This makes the solution practical as well as energy and water conscious.

The aim is to make water use visible, reduce wasted hot water, and encourage better habits every day. Although this is a safe foam-board model, the same sensing, feedback, and staged-control logic could be developed into a real smart-shower system.

Thank you for listening.