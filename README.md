# Arduino_ICOM_CI_V

The program is designed to control the frequency synthesizer according to the ICOM CI-V protocol.

To control the frequency of the synthesizer, you can use a program similar to OmniRig.

As the port, we indicate the serial port organized by Arduino itself.

As the type of transceiver, indicate Ike IC-706 (the brand of the transceiver can be changed inside the program).

The program implements only a few commands that control the frequency of the main local oscillator.

The necessary commands can be supplemented based on the directory. For instance - http://www.plicht.de/ekki/civ/civ-p0a.html

In the program, as an example, a synthesizer based on AD9850 is controlled.

The second version of the sketch implements support for the SI5351 synthesizer.

PS: Comments in the program in Russian. Sorry.
