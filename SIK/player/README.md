# Player

* Build
To build, simply `make`

* Usage
```{sh}
./player <address> <resource> <radio-port> <output> <metadata-flag>
```

Sample usages:
```{sh}
./player stream3.polskieradio.pl / 8900 - 10000 no | mplayer - [opcje]
./player stream3.polskieradio.pl / 8904 test3.mp3 30000 no
./player ant-waw-01.cdn.eurozet.pl / 8602 test5.mp3 50000 yes
```
