use std::ffi;
use std::path::Path;

use libc::*;
use log::{debug, error, info, trace, warn};

use crate::logger::CoreLogger;

#[repr(C)]
pub enum LogLevel {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Off,
}

#[no_mangle]
pub extern "C" fn logger_init(level: LogLevel) {
    // TODO: Init this from the C++ side
    CoreLogger::init(Path::new("log/rose-next.log"));

    let level = match level {
        LogLevel::Trace => log::LevelFilter::Trace,
        LogLevel::Debug => log::LevelFilter::Debug,
        LogLevel::Info => log::LevelFilter::Info,
        LogLevel::Warn => log::LevelFilter::Warn,
        LogLevel::Error => log::LevelFilter::Error,
        LogLevel::Off => log::LevelFilter::Off,
    };

    log::set_max_level(level);
}

#[no_mangle]
pub extern "C" fn logger_write(level: LogLevel, msg: *const c_char) {
    let msg_string = unsafe {
        ffi::CStr::from_ptr(msg as *mut c_char)
            .to_str()
            .unwrap_or("")
    };

    if msg_string.is_empty() {
        return;
    }

    match level {
        LogLevel::Trace => trace!("{}", msg_string),
        LogLevel::Debug => debug!("{}", msg_string),
        LogLevel::Info => info!("{}", msg_string),
        LogLevel::Warn => warn!("{}", msg_string),
        LogLevel::Error => error!("{}", msg_string),
        LogLevel::Off => {}
    }
}
