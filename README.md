[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)&nbsp;[![GitLab Pipeline](https://gitlab.com/acebrianjuan/gnss-sdr-monitor/badges/master/pipeline.svg)](https://gitlab.com/acebrianjuan/gnss-sdr-monitor/pipelines)&nbsp;[![Coverity Scan](https://scan.coverity.com/projects/19249/badge.svg)](https://scan.coverity.com/projects/acebrianjuan-gnss-sdr-monitor)

**Welcome to gnss-sdr-monitor!**

This program is a graphical user interface developed with Qt for monitoring the status of GNSS-SDR in real time.

![](./screenshots/gnss-sdr-monitor.png)

<table style="width:100%">
  <tr>
    <td><img src="./screenshots/gnss-sdr-monitor_constellation.png"/></td>
    <td><img src="./screenshots/gnss-sdr-monitor_cn0.png"/></td>
    <td><img src="./screenshots/gnss-sdr-monitor_doppler.png"/></td>
  </tr>
</table>

# How to build gnss-sdr-monitor
## Ubuntu 18.04 or above
### Install dependencies using software packages:

~~~~
$ sudo apt-get install build-essential cmake git libboost-dev libboost-system-dev \
       libprotobuf-dev protobuf-compiler qtbase5-dev qtdeclarative5-dev qtpositioning5-dev \
       libqt5charts5-dev qml-module-qtquick2 qml-module-qtquick-controls2 qml-module-qtquick-window2 \
       qml-module-qtlocation qml-module-qtpositioning qml-module-qtquick-layouts
~~~~

### Clone gnss-sdr-monitor Git repository:

~~~~~~
$ git clone https://github.com/acebrianjuan/gnss-sdr-monitor
~~~~~~

### Build and install gnss-sdr-monitor:

~~~~~~
$ cd gnss-sdr-monitor/build
$ cmake ..
$ make
~~~~~~

### Run gnss-sdr-monitor:

~~~~~~
$ cd src/
$ ./gnss-sdr-monitor
~~~~~~

