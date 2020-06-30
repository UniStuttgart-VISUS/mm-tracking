# Tracking Library `mm-tracking`

The `mm-tracking` library provides the interface that allows to communicate with the tracking system in front of the Powerwall at the Visualization Research Center (VISUS) of the University of Stuttgart. 
This library can be used for interaction with tracking devices (rigid-bodies) and button devices (stick, glasses).

---

## Building

Since the required library NatNet is not available for Linux and the library is only usable in combination with the Powerwall it only works with Windows.
[NatNet](http://optitrack.com/products/natnet-sdk/) is included in this package and it is required for this library to build. If it is not included download the SDK and copy the lib and include folder into the `../tracking/tracking/natnet/` folder. If NatNet is not automatically found set the appropriate `NATNET_GENERIC_LIBRARY`. 
[VRPN](https://github.com/vrpn/vrpn.git) is also required and it is automatically installed via `cmake`.

## Interface Classes

The tracking library interface provides the classes `Tracker` and `TrackingUtilizer`. 

The `Tracker` class includes on the one hand a VRPN client. It receives updates from the VRPN server about the button states of the button device(s). On the other hand it handles the connection to the NatNet server (Motive software) via the included NatNet client. The NatNet server streams the spatial data of the available rigid bodies recorded by the tracking cameras.
While the available rigid bodies are provided by the NatNet server, all available button devices have to be defined explicitly in the designated parameter list of the `Tracker`.
Only one `Tracker` class should be declared at a time.

The `TrackingUtilizer` manipulates the raw data from the `Tracker`. By changing the orientation of a pointing device while pressing the associated button, camera parameters can be manipulated. Further the intersection of the pointing device with the powerwall as well as the field of view is provided (in relative screen space coordinates). The class also allows to acccess the raw tracking data.
Multiple `TrackingUtilizers` can be connected to the `Tracker` simultaneously. Each `TrackingUtilizer` utilizes only one rigid body (motion or pointing device) and and button device. They are defined by their names in the corresponding parameters.

## Test Program

* Configure and generate projects with `cmake`.
* Adjust client IP parameter in `test/src/test.cpp` in line 35.
* Build all projects using `INSTALL` target in Viusal Studio. Tested for VS-15(2017) and VS-14(2015).
* Start 'Motive' software on NatNet server `mini`.
* Start VRPN server (`C:/vrpn/start64.bat`) on VRPN server `mini`.
* Place rigid body inside of tracking area.
* Start test program: `bin/test.exe`.

### Troubleshooting

#### Workaround for using non default compiler of Visual Studio with external project VRPN
If you want to use a compiler version other than the default provided with the chosen Visual Studio version, you have to explicitly add this information to the build command of VRPN. E.g. if you want to use Visual Studio 2017 and the Visual Studio 2015 compiler, you have to replace line 65 in `\tracking\CMakeLists.txt` as follows:
`CONFIGURE_COMMAND ${CMAKE_COMMAND} -G "Visual Studio 15 2017" -T v140 -DCMAKE_GENERATOR_PLATFORM=x64 -DVRPN_BUILD_CLIENTS:BOOL=OFF -DVRPN_BUILD_SERVERS:BOOL=OFF ${PROJECT_BINARY_DIR}/vrpn-prefix/src/vrpn{code}`

#### Missing firewall rule
If Windows doesn't ask to add a firewall exception for the test program, rules to allow incoming traffic for TCP port 3884 and UDP ports 1510 and 1511 have to be added manually.

## Documentation

See `documentation` folder.

## Parameters

The given default parameters in the example test program `test/src/test.cpp` fit the current VISUS tracking setup (Dezember 2019).

---
