Berikut adalah file README untuk proyek "Climate Change Monitoring System" yang menjelaskan fungsi dan dampak dari kode yang digunakan:

# Climate Change Monitoring System

## Overview

The Climate Change Monitoring System is designed to collect and transmit environmental data related to climate change. The system uses various sensors to measure parameters such as CO2 levels, temperature, humidity, irradiance, and energy consumption. The collected data is then sent to an MQTT broker for further analysis and visualization.

## Features

1. **Environmental Data Collection**:

   - **CCS811 Sensor**: Measures CO2 levels and total volatile organic compounds (TVOCs).
   - **DHT21 Sensor**: Measures temperature and humidity.
   - **Irradiance Sensor**: Measures solar irradiance.
   - **PZEM Sensor**: Measures electrical parameters such as voltage, current, power, and energy consumption.

2. **Data Transmission**:

   - The system connects to an Ethernet network using DHCP.
   - Data is published to an MQTT broker at regular intervals, ensuring that the information is available for real-time monitoring and historical analysis.

3. **Time Synchronization**:

   - The system uses an NTP (Network Time Protocol) client to synchronize time with an NTP server. This ensures that data is timestamped accurately according to the current time.

4. **Data Publishing**:
   - The system sends data to the MQTT broker every 5 minutes. Each data packet includes the latest readings from all sensors and a timestamp indicating when the data was collected.

## Components Used

- **Arduino Board**: The microcontroller unit used to interface with the sensors and handle data processing.
- **Ethernet Shield**: Provides network connectivity for the Arduino.
- **Sensors**: CCS811, DHT21, irradiance sensor, and PZEM sensor.
- **MQTT Broker**: A server that receives and stores data sent by the Arduino.

## Functionality

1. **Initialization**:

   - Upon startup, the system initializes the Ethernet connection, connects to the MQTT broker, and synchronizes time using the NTP client.

2. **Data Collection**:

   - The system reads data from the connected sensors, which measure various environmental parameters.

3. **Data Transmission**:

   - Every 5 minutes, the system creates a JSON payload containing the latest sensor readings and the current timestamp.
   - This JSON payload is published to the specified MQTT topic.

4. **Error Handling**:
   - The system includes error handling for Ethernet and MQTT connection issues, ensuring reliable operation.

## Impact

The Climate Change Monitoring System provides valuable data that can be used to analyze environmental changes over time. By continuously monitoring and transmitting data, the system supports efforts to understand and address climate change. The collected data can be used for:

- **Climate Research**: Providing insights into trends and patterns related to climate change.
- **Energy Management**: Monitoring energy consumption and efficiency.
- **Environmental Awareness**: Raising awareness about air quality, temperature, and other environmental factors.

## Usage

To deploy and use the Climate Change Monitoring System:

1. **Hardware Setup**:

   - Connect the sensors to the Arduino board according to their respective pin configurations.
   - Attach the Ethernet shield to the Arduino board.

2. **Software Configuration**:

   - Update the MQTT server address, port, and credentials in the code.
   - Ensure the NTP server settings are correctly configured.

3. **Deployment**:
   - Upload the code to the Arduino board using the Arduino IDE.
   - Monitor the serial output to verify the system is functioning correctly and sending data to the MQTT broker.
