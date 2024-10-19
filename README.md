# topd
Simple Unix daemon that logs system information to a file.

### Build & Run dependencies:
```
GCC
CMake
``` 
### Build & Run instructions.

1. Cloning the repository:
```
git clone https://github.com/dmytro7585/topd.git
cd topd
```
2. Building
```
mkdir build         
cd build
cmake --build .
```
3. Installing the program
```
sudo make install
```
4. Launching the program
```
sudo topd
```
5. An example of a launch with options: (This command will run the daemon and log CPU information and current time.)

```
sudo topd -t -c
```

6. Cleaning
To remove the generated files (object files and executable files), use the clean command:
```
cd build
make clean
```
7. Uninstalling the program.
```
cd build
sudo make uninstall
```
### Options:
```
- no options: logs CPU and GPU information
-h - displays help for all options
-t - logs the current time and date
-c - logs CPU information
-r - logs RAM information
-g - logs GPU information
-o - logs OS information
-m - logs monitor information
-d - logs disk space information
-i - logs internet speed
```

### Log File:

```
System information is logged to /tmp/topd.log.
```

### PID File:

```
Daemon’s PID is stored in /var/run/topd.pid.
```