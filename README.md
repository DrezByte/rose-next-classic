# Rose Online

## Setup
### Requirements
- Rust 1.38+
- Visual studio 2019 Community or better
- SQL Server Express 2014+
- SQL Server Management Studio

### Build
The project consists of a mix of Rust code and C++ code. In order to build the project one must
first build all the Rust dependencies and then build the C++ projects. The Rust crates must be
built using the 32-bit mscv toolchain. 

It's recommended to set the toolchain override for the entire `src/` directory using `rustup`. E.g.

```
cd src/
rustup rustup override set stable-i686-pc-windows-msvc
rustup show
```

Once all the Rust dependendencies are built then all the thirdparty C++ dependencies must be built.
This can be done by compiling the `thirdparty.sln` solution. Finally we can compile the remaining
projects by compiling the `rose-next.sln` solution.

#### Rust
- Change into the src directory `cd src/`
- Build all project `cargo build` and `cargo build --release`

### C++
- Open the `thirdparty.sln` solution and compile all projects
- Open the `rose-next.sln` solution and compile all the projects

## Client
### Debugging
The `client` project can be run from within Visual Studio to debug the executable. By default
it looks in the `game/` directory for the runtime game assets

### Connecting to servers
Run `trose.exe @TRIGGER_SOFT _server <IP>` to connect to a server at a specific IP Address

## Server
### Database
- Install SQL Server
- Enabled TCP/IP connection is SQL Server Configuration manager and ensure that the database is listening on the default port of `1433`
- Create 4 new databases `seven_ORA`, `SHO`, `SHO_LOG` and `SHO_MALL` with default settings
- Created 2 names user logins: `seven` and `sho`
    - User `seven` is required and the password is hardcoded into the login server. Set the password to: `tpqmsgkcm` or update the password in the login server lib (`sho_ls_lib/sho_ls_lib.cpp`)
    - User `sho` can have any password (recommended: `sho` password for dev)
- Run the 4 scripts from the `database/` directory (`seven_ora.sql`, `sho.sql`, `sho_log.sql`, `sho_mall.sql`)
- Enable `SQL Server and Windows Authentication` on your sql server (`Properties->Security`)
- Create 4 ODBC connections for each database using the `sho` user credentials. Ensure you are using the native client drivers.
    - See the `scripts/setup_odbc.cmd` for a convient way to setup the odbc connections

### Configuration
#### Data files
The servers require access to certain game data files. The server looks for these by default in a
folder called `data/` at the same level as the executable.

##### Development
- Run the `scripts/server_data.bat` script
- Update the server configuration file (`server.json`) to point to the data folder, e.g.

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
After building everything launch `1-login.bat` then `2-world.bat` then `3-game.bat` and finally `local.bat`

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
