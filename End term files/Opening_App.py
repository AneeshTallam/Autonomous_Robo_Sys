import serial
import subprocess
import os

# Update COM port as needed
ser = serial.Serial('COM5', 9600, timeout=1)

print("Listening for IR commands...")

while True:
    line = ser.readline().decode().strip()
    if not line:
        continue
    print("Received:", line)

    if line == "YTMUSIC":
        # Example: Launch YouTube Music Desktop App (adjust path)
        os.startfile(r"C:\Users\abdul\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Chrome Apps\YouTube Music.lnk")
    elif line == "NETFLIX":
        # Example: Launch Netflix Desktop App via Windows Shell command
        os.system('start shell:AppsFolder\\4DF9E0F8.Netflix_mcm4njqhnhss8!App')