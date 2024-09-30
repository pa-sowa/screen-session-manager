# Screen Session Manager

Screen Session Manager is a GUI application for managing `screen` sessions across various hosts via SSH. You can list screen sessions, view console output, quit, and start new sessions. This tool is ideal for users who frequently work with multiple `screen` sessions on different servers.

## Features

- **List screen sessions** from various remote hosts.
- **View console output** of individual screen sessions.
- **Start new screen sessions** on remote hosts.
- **Quit existing screen sessions**.

## Requirements

- Qt6
- `libssh`
- `screen` installed on remote hosts

## Installation

1. Clone the repository:

    ```bash
    git clone https://github.com/yourusername/ScreenSessionManager.git
    cd ScreenSessionManager
    ```

2. Build the project using CMake:

    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

3. Run the application:

    ```bash
    ./ScreenSessionManager
    ```

## Configuration

Currently, the UI for managing hosts is not implemented yet. You need to manually create a configuration file at `$HOME/.config/ScreenSessionManager/hosts.json` with the following content:

```json
{
    "default": "myserver.com",
    "hosts": [
        {
            "host": "localhost"
        },
        {
            "host": "myserver.com",
            "user": "john",
            "identityFile": "/home/john/.ssh/id_rsa"
        }
    ]
}
```
