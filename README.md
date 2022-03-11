# N2k-Airmar150wx-Compassfix

filter and substiture crappy output from Airmar 150Wx

using arduino due,  and both it's can controllers

-substiture wmm2020 variation

-filter out uncompensated compass heading

-filter out truewind direction (its calculated using uncompensated compass)

-add proper truewind direction calculated from other compass, and wmm2020 variation

-add truewind angle... (airmar didnt even supply it in the first place

-passthrough everything else coming from the airmar


N2k needed libraries:

https://github.com/ttlappalainen/NMEA2000

https://github.com/ttlappalainen/NMEA2000_due

https://github.com/ttlappalainen/CAN_BUS_Shield

WMM 2020 library

https://github.com/miniwinwm/WMM_Tiny


Due Flash storage libray

https://github.com/sebnil/DueFlashStorage

Time library

https://github.com/PaulStoffregen/Time
