# Licenses

## Libraries

| Name                                                   | License                                                                                 | Used by             | Released with            |
|--------------------------------------------------------|-----------------------------------------------------------------------------------------|---------------------|--------------------------|
| [argparse](https://github.com/cofyc/argparse)          | [MIT](https://github.com/cofyc/argparse/blob/master/LICENSE)                            | applications        | RDK                      |
| [bossa](https://github.com/shumatech/BOSSA)            | [BSD-3](https://github.com/shumatech/BOSSA/blob/master/LICENSE)                         | Hatvan flash tool   | RDK                      |
| [catch2](https://github.com/catchorg/Catch2)           | [Boost Software License 1.0](https://github.com/catchorg/Catch2/blob/devel/LICENSE.txt) | integration testing | not released             |
| [cmsis-dsp](https://github.com/ARM-software/CMSIS-DSP) | [Apache 2.0 License](https://github.com/ARM-software/CMSIS-DSP/blob/main/LICENSE.txt)   | ifxAngleMotion      | RDK                      |
| [mufft](https://github.com/Themaister/muFFT)           | [MIT](https://github.com/Themaister/muFFT/blob/master/COPYING)                          | ifxAlgo             | RDK, PAS CO2             |
| [mio](https://github.com/mandreyel/mio)                | [MIT](https://github.com/mandreyel/mio/blob/master/LICENSE)                             | ifxMmap             | RDK                      |
| [nlohmann](https://github.com/nlohmann/json)           | [MIT](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT)                        | applications        | RDK                      |
| [zip](https://github.com/kuba--/zip)                   | [Unlicense](https://github.com/kuba--/zip/blob/master/UNLICENSE)                        | ifxUtil             | RDK                      |
| [libnpy](https://github.com/llohse/libnpy)             | [MIT](https://github.com/llohse/libnpy/blob/master/LICENSE)                             | ifxDaq              | RDK                      |

Explanation of column "released with":
* RDK: The specific library is part of "Radar Development Kit" releases.
* PAS CO2: The specific library is part of PAS CO2 releases.
* not released: The specific library is only used internally and not part of any release.

## Tools

| Name                                                | License                                                              | used by                | Released with         |
|-----------------------------------------------------|----------------------------------------------------------------------|------------------------|-----------------------|
| [7zip](https://www.7-zip.org/)                      | [LGPLv2/BSD-3](https://www.7-zip.org/license.txt)                    | batch script in tools/ | not released          |
| [MinGW](http://winlibs.com/)                        | [multiple](https://winlibs.com/#license)                             | compilation on Windows | not released          |
