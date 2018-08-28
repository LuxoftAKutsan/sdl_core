# Run and test sdl_core on QNX7 x86

Open SDL may be ported on QNX7. 
And be runned on virtual x86 QNX7 workstation. 

## WBS:


##### I) Basic compilation and run
1. Compilation of 3rd party libraries for QNX. 
2. Refactoring of CMake structure (use already refactored in SYNC4 project) and use the toolchains for compilation
3. Creating USB transport adapter
4. Creating Bluetooth transport adapter

##### II) Automated testing 
5. Creating implementation RemoteTestingAdapterServer for QNX x86 ( SYNC4 implementation is already available):
 - Implementation of TCP mobile connection
 - Implementation of TCP Message brocke

6. Improove RemoteTestingAdapterServer and ATF to support remote SDL life cycle control
   Draft of proposal is available https://github.com/LuxoftAKutsan/sdl_evolution/blob/sdl_watchdog/proposals/nnn-atf-sdl-watchdog-service.md 

7. Create ATF module as Reley mobile applicaiton for support automation of USB and BT transport testing. 
   Draft of proposal is available https://github.com/LuxoftAKutsan/sdl_evolution/blob/additional_transports/proposals/NNNN-atf-additional-transports.md 
