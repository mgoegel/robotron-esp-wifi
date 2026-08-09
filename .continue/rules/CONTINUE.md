# CONTINUE Project Guide

## Step 1: Check Required Tools
First, verify that you have access to the necessary tools:
- file_glob_search: To discover project files
- read_file: To analyze file contents
- ls: To explore directory structure
- create_new_file: To generate the CONTINUE.md file
If any of these tools are unavailable, inform the user that they need to activate them and enable "Agent Mode" in Continue before proceeding.

## Step 2: Project Analysis
Analyze the project structure and key files to understand:
- The programming languages and frameworks used
- The overall architecture and organization
- Key components and their responsibilities
- Important configuration files
- Build/deployment system

## Step 3: Generate CONTINUE.md
Create a comprehensive CONTINUE.md file in the .continue/rules/ directory with the following sections:

1. **Project Overview**
   - Brief description of the project's purpose
   - Key technologies used
   - High-level architecture

2. **Getting Started**
   - Prerequisites (required software, dependencies)
   - Installation instructions
   - Basic usage examples
   - Running tests

3. **Project Structure**
   - Overview of main directories and their purpose
   - Key files and their roles
   - Important configuration files

4. **Development Workflow**
   - Coding standards or conventions
   - Testing approach
   - Build and deployment process
   - Contribution guidelines

5. **Key Concepts**
   - Domain-specific terminology
   - Core abstractions
   - Design patterns used

6. **Common Tasks**
   - Step-by-step guides for frequent development tasks
   - Examples of common operations

7. **Troubleshooting**
   - Common issues and their solutions
   - Debugging tips

8. **References**
   - Links to relevant documentation
   - Important resources

## 4. Project Overview

### Purpose
This project is a firmware for a device that uses a Wi-Fi adapter, providing functionalities like WLAN communication, web server management, VGA output, and OSD display.

### Key Technologies Used
- Programming Language: C
- Build System: CMake
- Frameworks: None specific (pure C)

### High-Level Architecture
The project is organized into components:
- `components\wifi-adapter`: Contains modules for WLAN, webserver, VGA, OSD, NVS, and keyboard handling.
- `html`: Contains HTML files for the web interface.
- `include`: Contains header files for all modules.

## 5. Getting Started

### Prerequisites
- CMake (version X.X or higher)
- A suitable development environment for embedded systems (e.g., ESP-IDF for ESP32 devices)
- Required libraries and dependencies

### Installation Instructions
1. Clone the repository.
2. Install CMake.
3. Configure the project using CMake:
   ```sh
   mkdir build
   cd build
   cmake ..
   ```
4. Build the project:
   ```sh
   make
   ```

### Basic Usage Examples
- To start the device with default settings:
  ```sh
  ./build/your_device_binary
  ```

- To access the web interface, connect to the device's Wi-Fi network and open a browser to the device's IP address.

### Running Tests
The project includes unit tests for individual modules. To run tests:
```sh
make test
```

## 6. Project Structure

### Main Directories
- `components`: Contains source code organized by functionality.
- `html`: HTML files for the web interface.
- `include`: Header files for all modules.

### Key Files and Their Roles
- `sdkconfig.old`: Configuration settings.
- `partitions.csv`: Memory partitioning definitions.
- `README.md`: General information about the project.
- `CMakeLists.txt`: Build configuration file.

### Important Configuration Files
- `sdkconfig.old`
- `partitions.csv`

## 7. Development Workflow

### Coding Standards or Conventions
- Use consistent naming conventions for functions and variables.
- Follow K&R style for C code formatting.
- Ensure proper commenting and documentation of functions and modules.

### Testing Approach
- Unit tests are written using a testing framework like CUnit.
- Integration tests cover interactions between different modules.
- Continuous integration is set up to automatically run tests on code changes.

### Build and Deployment Process
1. Configure the project with CMake.
2. Build the project using Make or another build tool.
3. Flash the binary onto the target device.
4. Verify functionality through the web interface and other interfaces.

### Contribution Guidelines
- Fork the repository and create a new branch for your feature or bug fix.
- Write unit tests for any new features or changes.
- Ensure that all code follows the coding standards.
- Submit a pull request with a clear description of the changes.

## 8. Key Concepts

### Domain-Specific Terminology
- **WLAN**: Wireless Local Area Network.
- **OSD**: On-Screen Display, used for overlaying information on video output.

### Core Abstractions
- Modules: Each module handles specific functionality like WLAN, webserver, VGA, OSD, NVS, and keyboard handling.

### Design Patterns Used
- Modular design: Each component is responsible for a specific part of the system.
- Layered architecture: The system can be divided into layers, each with defined responsibilities.

## 9. Common Tasks

### Step-by-Step Guides for Frequent Development Tasks

#### Adding a New Feature
1. Identify the appropriate module to add the feature to.
2. Create new source and header files if necessary.
3. Implement the feature in the new or existing files.
4. Write unit tests for the new feature.
5. Build and test the project.

#### Troubleshooting Common Issues

- **Build Errors**:
  - Check CMake configuration and dependencies.
  - Ensure that all required libraries are installed.

- **Web Interface Not Working**:
  - Verify Wi-Fi connectivity.
  - Check if the web server is running correctly.
  - Look for errors in the logs.

#### Updating Configuration
1. Edit `sdkconfig.old` or use the configuration tool provided by your development environment.
2. Rebuild the project to apply changes.

## 10. Troubleshooting

### Common Issues and Their Solutions
- **Build Errors**: Ensure all dependencies are installed and CMake is configured correctly.
- **Web Interface Not Accessible**: Check Wi-Fi connection and verify the web server's status.
- **Configuration Changes Not Applying**: Rebuild the project after making changes to configuration files.

### Debugging Tips
- Use debugging tools available in your development environment (e.g., GDB for ESP-IDF).
- Add debug statements to log important information during runtime.
- Verify that modules are correctly initialized and communicating with each other.

## 11. References

### Links to Relevant Documentation
- [CMake Documentation](https://cmake.org/documentation/)
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/)

### Important Resources
- `README.md`
- `sdkconfig.old`
- `partitions.csv`

This guide provides a comprehensive overview of the project, including setup instructions, development workflow, and troubleshooting tips. Please review and edit this file as needed to ensure it accurately reflects your project.