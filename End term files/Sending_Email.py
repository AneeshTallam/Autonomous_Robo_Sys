import serial
import yagmail

# Replace with your COM port (check Arduino IDE → Tools → Port)
ser = serial.Serial('COM3', 9600, timeout=1)

# Replace with your Gmail credentials
sender_email = 'abdullahbashir2014@gmail.com'
app_password = 'odmk vstl vigp fjgo'
receiver_email = 'abdullahalkenani99@gmail.com'

yag = yagmail.SMTP(sender_email, app_password)

print("Listening for message from Arduino...")

while True:
    line = ser.readline().decode().strip()
    if line == "SEND_EMAIL":
        print("⏰ Trigger received! Sending email...")
        yag.send(
            to=receiver_email,
            subject="Arduino Email Trigger",
            contents="Team 4 Arduino Email"
        )
        print("✅ Email sent.")
        break