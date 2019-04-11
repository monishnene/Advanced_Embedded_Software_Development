Example Summary
------------------
This application demonstrates how to use the ti.sysbios.knl.Clock module to
construct oneshot and periodic Clock instances. Clock Instances are
essentially functions that run after a certain number of Clock ticks.

Example Usage
--------------
Run the application, the two Clock instances will each print messages to the
console with their corresponding timeout parameters.

Application Design Details
-------------------------
The application is designed to run two Clock instances such that the peridoc
instance runs twice while the oneshot is only triggered once. As such, the
functions that are passed into the Clock instances, clkFxn and clk1Fxn
respectively are run when the instance reaches its prescribed timeout.

Notes
-----
To configure the Clock module, such as to set the systems tickPeriod in
microseconds, please see the applications' corresponding configuration file
clock.cfg.

Note: For IAR users using any SensorTag(STK) Board, the XDS110 debugger must be
selected with the 4-wire JTAG connection within your projects' debugger
configuration.

References
-----------
For more help, search either the SYS/BIOS User Guide or the TIRTOS
Getting Started Guide within your TIRTOS installation.