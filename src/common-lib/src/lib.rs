// Create a log facility
//  - Log to STDOUT
//  - Log to file based on date (`e.g. logs/mapserver-2019-21-04.log`)
//  - Log to a database based on action (`e.g. info!(target: 'game_event', "foo bar baz"))

// So my logging facility needs a way to specify:
//  - Log level
//  - Logging dir
//  - Database connection info
extern crate log;

pub mod ffi;
pub mod logger;
