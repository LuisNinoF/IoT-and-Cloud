# IoT-and-Cloud

## Objective
Demonstrate multiple applications for Cloud connectivity and the Internet of Things (IoT):

P1) WIFI bare metal app to transmit multi-sensor data to the cloud (ThingSpeak) over TCP using HTTP GET request.
Use: trend visualization.

P2) WIFI app to transmit data to own web server using an SQLite database over TCP using an HTTP POST request.
Use: sensor data analysis.

### Skills Learned

- WIFI implementation
- HTTP GET and POST requests
- Web development for data visualization
- Database design and query

### Tools Used

- STM32CubeIDE
- ThingSpeak
- Batcave

### Languages

- C
- HTML + CSS + JavaScript + PHP

### Hardware

- STM32F4
- ESP8266

### Data Visualization
- From P1, this example demonstrates sensor redundancy by comparing two sensor readings that should have the same value. If they differ significantly from each other, an alert can be triggered.
<img width="764" height="250" alt="ESP_Two_Sensors_1" src="https://github.com/user-attachments/assets/7f99bb7c-d6d5-4fbe-b066-b3f817aa8e54" />

- From P2, the screenshot below shows the direct comparison between two sensor readings both in chart and table form. It can be easily identified when one of the sensors has a significat deviation to take actions.
<img width="450" height="275" alt="Screenshot_4" src="https://github.com/user-attachments/assets/a102e04e-9e85-46b8-a078-4f60ae76880c" />
<img width="450" height="350" alt="Screenshot_6" src="https://github.com/user-attachments/assets/46f1d1bb-c5b9-4902-abb0-cfc0a3486124" />

