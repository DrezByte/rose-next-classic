#[derive(Debug)]
pub enum LauncherError {
    Message(String),
}

impl std::fmt::Display for LauncherError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            LauncherError::Message(s) => write!(f, "{}", s),
        }
    }
}

impl From<std::io::Error> for LauncherError {
    fn from(e: std::io::Error) -> LauncherError {
        LauncherError::Message(format!("IO Error: {}", e))
    }
}

impl From<std::str::Utf8Error> for LauncherError {
    fn from(e: std::str::Utf8Error) -> LauncherError {
        LauncherError::Message(format!("String Utf8 Error: {}", e))
    }
}
