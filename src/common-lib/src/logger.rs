use std::fs::{File, OpenOptions};
use std::io;
use std::io::Write;
use std::path::Path;
use std::sync::Mutex;

use log::{Metadata, Record};

pub struct CoreLogger {
    file: Mutex<File>,
}

impl CoreLogger {
    pub fn init(path: &Path) {
        let file = OpenOptions::new()
            .append(true)
            .create(true)
            .open(&path)
            .expect(&format!("Could not open log file: {}", path.display()));

        let logger = Box::new(CoreLogger {
            file: Mutex::new(file),
        });

        log::set_boxed_logger(logger).expect("Failed to create the logger");
    }
}

impl log::Log for CoreLogger {
    fn enabled(&self, metadata: &Metadata) -> bool {
        metadata.level() <= log::STATIC_MAX_LEVEL
    }

    fn log(&self, record: &Record) {
        if self.enabled(record.metadata()) {
            let msg = format!("[{}] {} \n", record.level(), record.args());

            let _ = io::stdout().lock().write_all(msg.as_bytes());

            if let Ok(mut f) = self.file.lock() {
                let _ = f.write_all(msg.as_bytes());
            };
        }
    }

    fn flush(&self) {}
}
