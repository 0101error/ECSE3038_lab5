# ECSE3038_lab5

## Summary of Functions

* `setup()`:  This function runs once at the start. It initializes the serial communication, LCD, connects to Wi-Fi using credentials from `env.h`, and displays "WiFi Connected!" on the LCD.
* `loop()`: This function runs repeatedly. It checks if Wi-Fi is connected, makes an HTTP GET request to the API endpoint `/message` with the API key from `env.h`, parses the JSON response, extracts "line_1" and "line_2", and displays them on the 16x2 LCD. If there are errors (Wi-Fi, HTTP, JSON parsing), it displays error messages on the Serial Monitor and LCD.

## Purpose of the Code

This code was written for the ECSE3038 Lab 5 assignment. The purpose is to demonstrate how to use an ESP32 to act as a web client, consume an API endpoint that returns a JSON response, parse the JSON data, and display a message on a connected 16x2 LCD.

## Riddle

Who spends money on a degree that stresses them out?

Ans: me 
