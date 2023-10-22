# senior_design
Senior design project

home ip: 192.168.86.248


__________Console Connection________________
To see available ports:
ls /dev/cu.*

To connect in (try one of these with the N replaced)
screen /dev/cu.SLAB_USBtoUART 115200
screen /dev/cu.usbserial-NNNN 115200

username: pi

_______Useful Raspi commands__________
To start hosting the flask server:
    export FLASK_APP=server.py
    flask run --host=0.0.0.0
raspi-config
ifconfig
ping