# IEEE Intro RTOS IEEE Example 

## Windows
- Have WSL installed and configured. See [this](https://learn.microsoft.com/en-us/windows/wsl/install) for instructions.
- Open Powershell and type the following command:

```powershell
wsl --install -d Ubuntu 
```

- In Windows Powershell as Administrator, run the following commands to use your USB in WSL

```powershell
<# Restart Shell after installation #>
winget install usbipd 

<# Continue in Powershell #>
usbipd list
usbipd bind --busid <BUSID>
usbipd attach --auto-attach --busid <BUSID> --wsl
```

- Follow Linux instructions below to install dependencies and setup Zephyr workspace.

## MacOS
- Install Homebrew if not already installed. See [this](https://brew.sh/) for instructions.
- Install the base dependencies using Homebrew:

```bash
brew install cmake ninja gperf python3 python-tk ccache qemu dtc libmagic wget openocd
```

- Follow Linux instructions below to install dependencies and setup Zephyr workspace.

## Linux 
- Install the STM32Cube Programmer. See [this](https://www.st.com/en/development-tools/stm32cubeprog.html) for instructions.

- Add the programmer to your PATH. For example, if you installed it in `/opt/stm32cubeprog`, add the following line to your `~/.bashrc` or `~/.zshrc`:

```bash 
    export PATH=$PATH:STM32-Programmer/bin
```

- Using the handy makefile provided, you can setup all your tools by running the following: 

```bash
make setup
```

- To build the project, run the following command:

```bash
make build
```

- To flash the project to your board, run the following command:

```bash
make prog
```
