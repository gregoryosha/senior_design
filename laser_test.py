import RPi.GPIO as GPIO
import time


KY010 = 7
LED = 12


def toggle_led(null):  # Function to toggle LED on/off when beam is blocked
    if GPIO.input(KY010):
        GPIO.output(LED, GPIO.LOW)  # If beam is blocked, turn LED down
    else:
        GPIO.output(LED, GPIO.HIGH)  # If beam is not blocked, turn LED on


GPIO.setmode(GPIO.BOARD)
GPIO.setup(KY010, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(LED, GPIO.OUT, initial=GPIO.HIGH)

# When the beam is blocked/unblocked call the function to turn LED on/off
GPIO.add_event_detect(KY010, GPIO.BOTH, callback=toggle_led, bouncetime=100)

print("Use CTRL+C to exit.")

try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    GPIO.cleanup()
