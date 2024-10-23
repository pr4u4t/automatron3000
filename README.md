# Automatron 3000
Automatron 3000 is a powerful data presentation and analysis tool designed for real-time monitoring and visualization. It allows seamless communication with various data sources such as Serial Port, LIN Bus, CAN, and ESP32 sensors. The data is displayed using interactive plots, meters, and gauges, making it ideal for monitoring systems, automotive diagnostics, sensor analysis, and much more.

## Features
* Data Acquisition:
    * Supports multiple data sources: Serial Port, LIN Bus, CAN, and ESP32 sensors.
    * Real-time data retrieval with minimal latency.
* Visualizations:
    * Interactive plots for trend analysis.
    * Customizable meters and gauges for monitoring key data points.
    * Real-time updates to visual elements.
    * Conversion of measurement values ​​in real time.
* Flexible and Extensible:
    * Easily customizable and extendable for additional data sources and meters through plugins.
    * Scalable architecture for handling a variety of datasets.
* Cross-Platform:
    * Built using Qt, ensuring compatibility across major platforms like Windows, macOS, and Linux.

## Supported Protocols:
* Serial Port: Reads data from standard RS232/RS485 interfaces.
* LIN Bus: Low-speed serial communication for automotive applications.
* CAN Bus: High-speed bus communication, widely used in automotive and industrial systems.
* Arduino, ESP32: Integration with sensors via wireless or wired ethernet.

## Plugins:

### QCustomAction
The CustomAction plugin allows you to invoke methods from other plugins seamlessly. It includes optional features such as a progress bar and log window for tracking execution. For the methods to be callable, they must be marked with Q_INVOKABLE

### QBadge
The QBadge plugin enables the display of customizable badges with text descriptions, titles, and images, providing a simple way to visually represent information or status.

### QCircularBar
The QCircularBar plugin is a gauge that visualizes sensor data in a circular format. It features configurable units, maximum values, and threshold settings, offering a clear and customizable way to monitor sensor readings.

### QJTAG
The QJTAG plugin enables the execution of configurable programs with customizable arguments, providing flexibility for running external processes directly from within the application.

### QKonsole
The QKonsole plugin is a serial port terminal that allows you to send and receive commands, facilitating communication over serial connections for device control and data exchange.

### QLin
The QLin plugin is a highly configurable LIN bus I/O device that operates using the Vector LIN USB interface, enabling seamless communication and control for LIN-based systems.

### QLinBus
The QLinBus plugin is a LIN bus scanner and sniffer that monitors and analyzes LIN bus communication using any available LIN bus I/O device, providing real-time data capture and diagnostics.


## Usage

1. Select Data Source:

    * Connect the tool to the desired data source (Serial Port, LIN, CAN, ESP32).

2. Configure Visualization:

    * Set up the visualization by choosing between available plots, meters, or gauges.

3. Start Monitoring:

    * Begin real-time data monitoring and adjust parameters as needed for analysis.

## Contributing
Contributions are welcome! If you'd like to help improve Automatron 3000, please follow these steps:

1. Fork the repository.
2. Create a new branch (git checkout -b feature-branch).
3. Commit your changes (git commit -m 'Add new feature').
4. Push to the branch (git push origin feature-branch).
4. Open a pull request.

## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Contact
For any questions or suggestions, feel free to contact the project maintainer:

Name: Paweł Ciejka\
Email: pawel.ciejka@gmail.com
