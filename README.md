# IoT-and-Cloud

## Objective
Demonstrates bare-metal WiFi implementations on STM32 for transmitting multi-sensor data to cloud platforms across three applications:

P1) ThingSpeak via TCP/HTTP GET for real-time trend visualization and sensor redundancy analysis.

P2) Custom web server with SQLite database via TCP/HTTP POST for data storage and analysis.

P3) AWS IoT (DynamoDB NoSQL, Lambda, API Gateway) via HTTPS POST/GET for secure transmission, storage, and verification.-

### Skills Learned

- Bare-metal WiFi stack (ESP AT commands)
- HTTP GET/POST and HTTPS (TLS over TCP)
- RESTful API design and CRUD operations
- Database integration (SQLite, DynamoDB NoSQL)
- Serverless AWS architecture (Lambda, API Gateway)
- Web development for data visualization

### Tools Used

- STM32CubeIDE
- ThingSpeak
- Batcave
- AWS: DynamoDB, Lambda, API Gateway
- Postman API platform

### Languages

- C (STM32F4 firmware)
- HTML + CSS + JavaScript + PHP (P2 web development)
- Python (AWS Lambda)

### Hardware

- STM32F4 (Microcontroller and ADC sensors)
- ESP8266/ESP32 (WiFi modules)

### Data Visualization
- From P1, this example demonstrates sensor redundancy by comparing two sensor readings that should have the same value. If they differ significantly from each other, an alert can be triggered.
<img width="764" height="250" alt="ESP_Two_Sensors_1" src="https://github.com/user-attachments/assets/7f99bb7c-d6d5-4fbe-b066-b3f817aa8e54" />

- From P2, the screenshot below shows the direct comparison between two sensor readings both in chart and table form. It can be easily identified when one of the sensors has a significat deviation to take actions.
<img width="450" height="275" alt="Screenshot_4" src="https://github.com/user-attachments/assets/a102e04e-9e85-46b8-a078-4f60ae76880c" />
<img width="450" height="350" alt="Screenshot_6" src="https://github.com/user-attachments/assets/46f1d1bb-c5b9-4902-abb0-cfc0a3486124" />

- From P3, the screeshot of the DynamoDB table shows how the data is securely transmited and stored.
<img width="566" height="347" alt="Screenshot 1" src="https://github.com/user-attachments/assets/eb5e8c82-2c5b-421b-90ef-3894dae7f761" />


