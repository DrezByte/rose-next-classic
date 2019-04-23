use log::{Metadata, Record};
use lazy_static::lazy_static;

lazy_static! {
    pub static ref LOGGER: CoreLogger = CoreLogger::new();
}

pub struct CoreLogger;

impl CoreLogger {
    pub fn new() -> CoreLogger {
        CoreLogger {}
    }
}

impl log::Log for CoreLogger {
    fn enabled(&self, metadata: &Metadata) -> bool {
        metadata.level() <= log::STATIC_MAX_LEVEL
    }

    fn log(&self, record: &Record) {
        if self.enabled(record.metadata()) {
            println!("{} - {}", record.level(), record.args());
        }
    }

    fn flush(&self) {}
}
