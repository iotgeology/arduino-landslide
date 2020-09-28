# arduino-landslide

EMMS Lab Experiment
Earth Movement Monitoring System (EMMS) 

Laboratory Experiment in September 2019 
conducted by Jon Barnes

This system monitors Earth Movement (Landslides) at seperate Sensor Nodes and relays the information back to the Base Unit via 2.4ghz radio (utilizing data packets checked with CRN over a tree network). This project uses; Arduino Unos, Accelerometers and Gyroscopes (MPU 6050 on GY-521), nrf24L01+ radios, and micro sd card.

This project uses 2 different sensor network node types; (1) the individual sensors and (2) the main hub. 
The individual sensors collect the data and send it over 2.4ghz packet radio over a mesh network to the main hub. The main hub receives the data amd stores it in an sd card.
