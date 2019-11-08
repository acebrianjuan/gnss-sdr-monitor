[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![GitLab Pipeline](https://gitlab.com/acebrianjuan/gnss-sdr-monitor/badges/master/pipeline.svg)](https://gitlab.com/acebrianjuan/gnss-sdr-monitor/pipelines)
[![Coverity Scan](https://scan.coverity.com/projects/19249/badge.svg)](https://scan.coverity.com/projects/acebrianjuan-gnss-sdr-monitor)

**Welcome to gnss-sdr-monitor!**

This program is a graphical user interface developed with [Qt](https://www.qt.io/) for monitoring the status of [GNSS-SDR](https://gnss-sdr.org) in real time.

![](./screenshots/gnss-sdr-monitor.png)

<table style="width:100%">
  <tr>
    <td><img src="./screenshots/gnss-sdr-monitor_constellation.png"/></td>
    <td><img src="./screenshots/gnss-sdr-monitor_cn0.png"/></td>
    <td><img src="./screenshots/gnss-sdr-monitor_doppler.png"/></td>
  </tr>
</table>

## How to build gnss-sdr-monitor

### Install dependencies using software packages:

#### Debian / Ubuntu

If you are using Debian 10, Ubuntu 18.04 or above, this can be done by copying and pasting the following line in a terminal:

~~~~
$ sudo apt install build-essential cmake git libboost-dev libboost-system-dev \
       libprotobuf-dev protobuf-compiler qtbase5-dev qtdeclarative5-dev qtpositioning5-dev \
       libqt5charts5-dev qml-module-qtquick2 qml-module-qtquick-controls2 qml-module-qtquick-window2 \
       qml-module-qtlocation qml-module-qtpositioning qml-module-qtquick-layouts
~~~~

Once you have installed these packages, you can jump directly to [download the source code and build gnss-sdr-monitor](#download-and-build-linux).

#### Arch Linux

If you are using Arch Linux:

~~~~
$ pacman -S gcc make cmake git boost boost-libs protobuf qt5-base qt5-declarative qt5-location \
       qt5-charts qt5-quickcontrols2
~~~~

Once you have installed these packages, you can jump directly to [download the source code and build gnss-sdr-monitor](#download-and-build-linux).

#### CentOS

If you are using CentOS 7, you can install the dependencies via Extra Packages for Enterprise Linux ([EPEL](https://fedoraproject.org/wiki/EPEL)):

~~~~
$ sudo yum install wget
$ wget https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
$ sudo rpm -Uvh epel-release-latest-7.noarch.rpm
$ sudo yum install gcc-c++ make cmake git boost-devel protobuf-devel protobuf-compiler \
       qt5-qtbase-devel qt5-qtdeclarative-devel qt5-qtlocation-devel qt5-qtcharts-devel \
       qt5-qtdeclarative-devel
~~~~

Once you have installed these packages, you can jump directly to [download the source code and build gnss-sdr-monitor](#download-and-build-linux).

#### Fedora

If you are using Fedora 28 or above, the required software dependencies can be installed by doing:

~~~~
$ sudo dnf install gcc-c++ make cmake git boost-devel protobuf-devel protobuf-compiler \
       qt5-qtbase-devel qt5-qtdeclarative-devel qt5-qtlocation-devel qt5-qtcharts-devel \
       qt5-qtdeclarative-devel
~~~~

Once you have installed these packages, you can jump directly to [download the source code and build gnss-sdr-monitor](#download-and-build-linux).

#### openSUSE

If you are using openSUSE Leap or openSUSE Tumbleweed:

~~~~
$ zypper install gcc-c++ cmake git boost-devel protobuf-devel libqt5-qtnetworkauth-devel \
       libQt5PrintSupport-devel libQt53DQuick-devel libqt5-qtlocation-devel libQt5Charts5-devel
~~~~

Once you have installed these packages, you can jump directly to [download the source code and build gnss-sdr-monitor](#download-and-build-linux).

### <a name="download-and-build-linux">Clone gnss-sdr-monitor Git repository</a>:

~~~~~~
$ git clone https://github.com/acebrianjuan/gnss-sdr-monitor
~~~~~~

### Build and install gnss-sdr-monitor:

~~~~~~
$ cd gnss-sdr-monitor/build
$ cmake ..
$ make
~~~~~~

This will create the `gnss-sdr-monitor` executable at the gnss-sdr-monitor/src directory. You can run it from that folder, but if you prefer to install `gnss-sdr-monitor` on your system and have it available anywhere else, do:

~~~~~~
$ sudo make install
~~~~~~

### Run gnss-sdr-monitor:

~~~~~~
$ cd src/
$ ./gnss-sdr-monitor
~~~~~~

