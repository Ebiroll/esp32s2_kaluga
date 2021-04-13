


An esp32-s2 based 433.92 Mhz switcer with focus to turn everything off.


Uses RFM96 OOK library

https://github.com/kobuki/RFM69OOK

Uses the RFM96 chip
https://www.hoperf.com/data/upload/portal/20190801/RFM96W-V2.0.pdf

Image of connection,
https://camo.githubusercontent.com/ee245ee5a81e1f6ab9a377545beb1b7cbc359b1b8ad239b6893f9b3f839c1a0f/687474703a2f2f692e696d6775722e636f6d2f7462476a4c75392e6a7067




To make things easier, use Arduino as a component
    mkdir -p components && \
    cd components && \
    git clone https://github.com/espressif/arduino-esp32.git arduino && \
    cd arduino && \
    git submodule update --init --recursive && \
    cd ../.. 


    idf.py set-target esp32s2
    idf.py menuconfig


# However for the esp32s2 I had to add this in menuconfig

    error: #error "Please configure IDF framework to include mbedTLS -> Enable pre-shared-key ciphersuites and activate at least one cipher"
 #  error "Please configure IDF framework to include mbedTLS -> Enable pre-shared-key ciphersuites and activate at least one cipher"
    
       Choose SSL/TLS library for ESP-TLS (See help for more Info) (mbedTLS)  --->
[*] Use Digital Signature (DS) Peripheral with ESP-TLS
[ ] Enable ESP-TLS Server
[*] Enable PSK verification
[ ] Allow potentially insecure options


