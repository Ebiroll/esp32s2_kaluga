


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
    cd ../.. && \
    make menuconfig