# System Internals and Cybersecurity Coursework

This repository contains three coursework projects created as part of the System Internals and Cybersecurity modules at university.

## Assessment 1

The project has been split into separate files. The `main.c` file contains functionality to display the menu and handle user input. The `machinecontrol.c` file contains the functionality for adding, removing, editing, saving, and loading machines. The `compile.sh` file contains the compile command that links the files together. The `main.c` file has access to the `machinecontrol.c` functions through the `machinecontrol.h` header file. Global definitions (includes max machines, name length, location length, and data file location) are stored in a `defines.h` header file so that they can be used across all files. This design allows for separation of concerns and makes for clean code.

The machines are defined in a data structure and stored in a dat file. The machines are read and saved by using the `fread()` and `fwrite()` functions respectively. When the program is started, all machines are loaded from data file into memory. This improves performance because it reduces the number of IO file operations. There is a max machine limit to prevent a malicious user from filling the dat file with an excessive number of records. The `system()` function is used to execute the `gpio` Linux command that turns on the LED by writing to the specified machine pin.

`Sscanf()` is known to be insecure and susceptible to buffer overflow attacks. Therefore, `fgets()` has been used as opposed to `sscanf()` for user input. In `fgets()`, the max input length can be specified to limit the number of characters read from the input stream. If the user inputs data that is too long they will be notified with an error message. Then, `fgets()` is run in a loop until a newline character is found so the buffer can be cleared to ensure the leftover input doesn’t get read in the next operation. Numeric user input is parsed using `strtol()` as opposed to `strtoi()` to accommodate for cases where the user enters an exceptionally long number. To protect the hardware, the input pin when creating or updating a machine is verified to ensure it is a valid GPIO pin. The application will not fail when the user enters exceptional data and thus should not be susceptible to a buffer overflow attack.

A current issue with the user input is that when testing on Linux and inputting a blank character by pressing `CTRL+D`, it sends the program into an infinite loop asking for input. 

The program was run through the Flawfinder security analysis tool. It picked up issues with the use of `strcpy()` because it can be exploited with a buffer overflow attack due to the dest and src params not being compared for length equality. To prevent this, `strncpy()` has been used because the string max length can be specified. `strncpy()` cannot guarantee the string will be null terminated. The alternative is to use `strlcpy()` but as it is not standard in the C library, it would make for less portability. Nevertheless, the issue should not arise because the user input is checked for the correct length before being passed to the function.

## Assessment 2

### LKM

IOCTL command numbers should be unique across the system to prevent the case where the correct command is issued to the incorrect device which would result in unexpected and perhaps dangerous behaviour. This is done using a magic number and the `_IOR` and `_IOW` functions when defining the commands. To avoid conflicts, a unique magic number was found by consulting the `ioctl-number.txt` file. The IOCTL handler has been updated to return `-ENOTTY` when an invalid command is specified as per POSIX standards. The `access_ok()` function is used to verify the user address after receiving a pointer from `copy_from_user()`.

Being a single-open device, the kernel module is limited because it can only be used by one process at a time. This would be improved by allowing concurrent usage of the device. 

Originally, the open function on the device would return an error if the device was busy. This has been replaced with a spinlock so the calling process will wait until the driver is not in use. Using the spinlock will make the calling process wait until the device is free as opposed to returning an error when the device is busy. This is fine in this project because the IO operations are quick, but depending on the policy it may not be suitable to wait. Creating a device for each access policy can accommodate for varying needs. Using spinlock here is useful because a terminal process could be executing the toggle command while the web control panel can read a value at the same time. The same effect could have been achieved by simply removing the `DEV_BUSY` logic, however that would leave the device susceptible to race conditions where data could be overwritten, particularly the `lkmdata` and `apin` global variables. 

A more effective way of implementing concurrency would be to clone the device on open to create a virtual device for each process.


### Web

An Apache web server on the Pi hosts a simple PHP page that allows the user to read, write, and toggle pins. When the HTML form is submitted, the `shell_exec()` PHP function is called to execute the test app commands. The user input is sanitised to prevent shell command injection.

### Test App

The user space application has been placed into the `/usr/bin` folder so it can be added to the PATH meaning it can be executed with just using the `piio` command. 

The toggle functionality has been implemented on the test app with the philosophy of keeping the LKM “mechanism over policy”. The LKM defines what capabilities there are whereas the test app defines how the capabilities can be used. Writing this functionality in the user space also gives it more security because failures are more catastrophic in a kernel module. 

The toggle function does not implement more `ioctl()` usage, it simply executes the write_to_driver_gpio() ` function in the test app every x microseconds for y times and uses the `usleep()` function.

## Assessment 3

### Setting up EC2 Instance on AWS 

An EC2 instance running Ubuntu 18.04 was created with AWS on the `t2.micro` tier. A name tag was assigned to the instance and the attached storage volume so that they can be easily identified in the case that event logs need to be checked. It is vital that remote access to the instance is secured and so a public and private keypair was generated for connecting via SSH. An elastic IP was requested and assigned to the instance which is useful because the client scripts will not need to be updated with a new IP in the event that either the instance dynamic IP changed, or a new instance was launched.  

The principle of least privilege was followed when setting up this server. A security group was created and assigned to the server with only inbound traffic allowed on ports 22 and 8883 for SSH and MQTT, respectively. Access to these ports was restricted to a specific IP; in this case the IP of the machine that the subscribe and publish scripts will be executed from. This prevents unknown sources from attempting to communicate with the server. 

The Ubuntu server was upgraded to the latest version by running the relevant update and upgrade commands. This ensures that the operating system and applications are running at the latest versions which prevents security issues caused by outdated software. 

An Amazon Machine Image (AMI) of the fully working instance running Ubuntu and MQTT was saved in the event of a disaster that requires the machine to be rebuilt. This process also created a snapshot for the storage volume attached to the instance. 

### Installing and Securing the MQTT Broker 

The Mosquitto package was installed through the Ubuntu package manager. This allows for the MQTT service to be run on the server. The MQTT broker was secured with username and password authentication which was setup using the `mosquitto_passwd` command. The config file was updated to disallow anonymous connections and to specify the location of the password file. The broker was further secured with SSL/TLS by creating a CA certificate with the `openssl` command. A self-signed certificate was used in this instance for demonstration purposes but would not be suitable for use in a production environment. Additionally, the config file was updated to specify port 8883 the default listening port as per MQTT standards. 

### Publish and Subscribe Scripts 

Two Python scripts, `publish.py` and `subscribe.py`, were created using the Paho library which has been designed to allow communication with an MQTT broker. It is vital that communications between the client and server are encrypted as they may contain sensitive data. This has been achieved by using TLS certificates previously detailed in this report. 

The subscribe script connects to the MQTT broker and subscribes to two topics: `piio/writepin`, and `piio/toggle`. When this subscriber receives a message on one of these topics, it checks the payload to ensure that it contains the expected JSON values and then the relevant `piio` command will be executed with the specified parameters using Python’s OS library functions. The publish script takes in user input for reading, writing, or toggling a pin for the PIIO user application created in previous parts of this module. The user input is validated and put into JSON format before being published to the MQTT broker. 
