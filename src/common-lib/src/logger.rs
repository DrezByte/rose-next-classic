use std::fs::{create_dir_all, File, OpenOptions};
use std::io;
use std::io::Write;
use std::path::{Path, PathBuf};
use std::sync::Mutex;

use chrono::{Date, Utc};
use log::{Metadata, Record};

pub struct CoreLogger {
    path: PathBuf,
    timestamp: Date<Utc>,
    file_stream: Mutex<io::BufWriter<File>>,
}

impl CoreLogger {
    pub fn init(path: &Path) {
        create_dir_all(path.parent().unwrap())
            .expect("Failed to create intermediate dirs for logs");

        let logger = Box::new(CoreLogger {
            path: path.to_path_buf(),
            timestamp: Utc::today(),
            file_stream: Mutex::new(CoreLogger::build_file_stream(path)),
        });

        log::set_boxed_logger(logger).expect("Failed to create the logger");
    }

    pub fn build_file_stream(path: &Path) -> io::BufWriter<File> {
        let file_stem = path
            .file_stem()
            .unwrap_or_default()
            .to_str()
            .expect(&format!(
                "No valid file stem for log file at: {}",
                path.display()
            ));

        let file_ext = path
            .extension()
            .unwrap_or_default()
            .to_str()
            .unwrap_or_default();

        let new_file_stem = format!("{}-{}", file_stem, Utc::today().format("%Y-%m-%d"));
        let mut open_path = PathBuf::from(path);
        open_path.set_file_name(new_file_stem);
        open_path.set_extension(file_ext);

        let file = OpenOptions::new()
            .append(true)
            .create(true)
            .open(&open_path)
            .expect(&format!("Could not open log file: {}", path.display()));

        io::BufWriter::new(file)
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

            if let Ok(mut file_stream) = self.file_stream.lock() {
                // Rotate log file
                if Utc::today() != self.timestamp {
                    let open_path = PathBuf::from(&self.path);
                    *file_stream = CoreLogger::build_file_stream(&open_path);
                }

                if let Ok(()) = file_stream.write_all(msg.as_bytes()) {
                    file_stream.flush().expect("Error flushing log buffer");
                }
            };
        }
    }

    fn flush(&self) {
        if let Ok(mut file_stream) = self.file_stream.lock() {
            file_stream.flush().expect("Error flushing log buffer");
        }
    }
}
