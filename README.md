[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](pvr.plutotv/LICENSE.txt)
[![Build Status](https://travis-ci.org/flubshi/pvr.plutotv.svg?branch=Matrix)](https://travis-ci.org/flubshi/pvr.plutotv)
[![Build Status](https://dev.azure.com/flubshi/pvr.plutotv/_apis/build/status/flubshi.pvr.plutotv?branchName=Matrix)](https://dev.azure.com/flubshi/pvr.plutotv/_build/latest?definitionId=1&branchName=Matrix)
[![Build Status](https://jenkins.kodi.tv/buildStatus/icon?job=flubshi%2Fpvr.plutotv%2FMatrix)](https://jenkins.kodi.tv/job/flubshi/job/pvr.plutotv/job/Matrix/)

# Pluto.tv PVR client for Kodi
This is the Pluto.tv PVR client addon for [Kodi](https://kodi.tv). It provides Kodi integration for the TV streaming provider pluto.tv

## Preview Images

<img src="pvr.plutotv/resources/screenshots/screenshot-01.jpg" width="300" /> <img src="pvr.plutotv/resources/screenshots/screenshot-02.jpg" width="300" />

## Installation

- You need to compile from source.

## Disclaimer

This is an *unofficial* plugin. It is provided by volunteers and not related to pluto.tv.
For any support regarding this plugin, please create a github issue.


## Build instructions

### Linux

1. `git clone --branch master https://github.com/xbmc/xbmc.git`
2. `git clone --branch Matrix https://github.com/flubshi/pvr.plutotv.git`
3. `mkdir -p  xbmc/cmake/addons/addons/pvr.plutotv/`
4. `echo "pvr.plutotv https://github.com/flubshi/pvr.plutotv master" > xbmc/cmake/addons/addons/pvr.plutotv/pvr.plutotv.txt`
5. `cd pvr.plutotv && mkdir build && cd build`
6. `cmake -DADDONS_TO_BUILD=pvr.plutotv -DADDON_SRC_PREFIX=../.. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../../xbmc/addons -DPACKAGE_ZIP=1 ../../xbmc/cmake/addons`
7. `make`

