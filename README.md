# Rose Online

## Setup
### Requirements
- Rust 1.38+
- Visual studio 2019 Community or better
    - "Desktop development with C++" bundle is recommended
- SQL Server Express 2014+
- SQL Server Management Studio
- PostgreSQL 12+
- Python 3+ (for scripts)
- Clang-format (for code formatting)

### Build
The project consists of a mix of Rust code and C++ code. In order to build the project one must
first build all the Rust dependencies and then build the C++ projects. The Rust crates must be
built using the 32-bit mscv toolchain. 

It's recommended to set the toolchain override for the entire `src/` directory using `rustup`. E.g.

```
cd src/
rustup override set stable-i686-pc-windows-msvc
rustup show
```

Once all the Rust dependendencies are built then all the thirdparty C++ dependencies must be built.
This can be done by compiling the `thirdparty.sln` solution. Finally we can compile the remaining
projects by compiling the `rose-next.sln` solution.

#### Rust
- Change into the src directory `cd src/`
- Install the 32-bit msvc toolchain `rustup toolchain install  stable-i686-pc-windows-msvc`
- Set the toolchain override `rustup override set stable-i686-pc-windows-msvc`
- Build all project `cargo build` and `cargo build --release`

### C++
- Open the `thirdparty.sln` solution and compile all projects
- Open the `rose-next.sln` solution and compile all the projects

> Note: Don't forget to build both configurations (debug/release)

## Client
### Debugging
The `client` project can be run from within Visual Studio to debug the executable. By default
it looks in the `game/` directory for the runtime game assets

### Connecting to servers
Run `rosenext.exe --server <IP>` to connect to a server at a specific IP Address

#### Auto-connect
The client has an extra feature to automatically connect directly to the game
world. It requires that all the servers are running and that a character exists
and is known by name. E.g. usage:

```
rosenext.exe --server 127.0.0.1 --username user --password pass --auto-connect-server 1 --auto-connect-channel 1 --auto-connect-character MyChar
```

## Server
### Database
- Install SQL Server
- Create 2 new databases `SHO` and `SHO_LOG` with default settings
- Created a named user logins: (default is username `seven` and password `tpqmsgkcm` but this can be overriden in `server.toml`)
- Run the 2 scripts from the `database/` directory (`sho.sql` and `sho_log.sql`)
- Create 2 ODBC connections for each database using windows authentication.
    - See the `scripts/setup_odbc.cmd` for a convient way to setup the odbc connections

### Configuration
#### Data files
The servers require access to certain game data files. The server looks for these by default in a
folder called `data/` at the same level as the executable.

##### Development
- Run the `scripts/server_data.bat` script
- Run the `scripts/generate_launchers.cmd` script for convenience scripts
- Update the server configuration file (`server.toml`) to point to the data folder, e.g.

```toml
[worldserver]
data_dir = "C:\\dev\rose-next\server\data"

[gameserver]
data_dir = "C:\\dev\rose-next\server\data"
```

### New user
Insert a row into `dbo.UserInfo`, the following fields are required:
- `Account` - The user's login name
- `Email` - The user's email address
- `MD5PassWord` - MD5 encoded version of user's password
- `MailIsConfirm` - Must be true for user to log in
- `Right` - The user permissions (0 - cant login, 1 - default, 768 - full admin)

### Start
After building everything launch `1-login.bat` then `2-world.bat` then `3-game.bat` and finally `client.bat`

Alternatively the servers can be invoked manually from the command line with an
optional flag to the config file

```cmd
sho_loginserver.exe --config C:\rose\loginserver.toml
sho_worldserver.exe --config C:\rose\worldserver.toml
sho_gameserver.exe --config C:\rose\gameserver.toml
```

### Config file
The servers have default values but can optionally read their values from a `server.toml` file in the
same directory as the executable.

Log levels can be controlled using the values 0-5. With 0 being the highest logging level and 5 being the lowest (no logs)
```
# Log levels
0 - Trace
1 - Debug
2 - Info
3 - Warn
4 - Error
5 - Off
```

```toml
[database]
ip = "127.0.0.1"
name = "SHO"
username = "seven"
password = "tpqmsgkcm"

[loginserver]
ip = "127.0.0.1"
port = 29000
server_port = 19000
password = "password"
log_level = 2
log_path = "C:\\rose\\logs\\loginserver.log"

[worldserver]
ip = "127.0.0.1"
port = 29100
server_port = 19001
world_name = "1Rose Next" # Prepend number to control ordering in world list
data_dir = "C:\\rose\data\\"
log_level = 2
log_path = "C:\\rose\\logs\\worldserver.log"

[gameserver]
ip = "127.0.0.1"
port = 29200
server_name = "Channel 1"
data_dir = "C:\\rose\\data\\"
log_level = 2
log_path = "C:\\rose\\logs\\worldserver.log"
```
