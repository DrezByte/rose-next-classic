use std::collections::HashMap;
use std::error::Error;
use std::fs;
use std::fs::File;
use std::io;
use std::path::{Path, PathBuf};
use std::time;

use bincode;
use clap::{App, Arg, ArgMatches, SubCommand};
use serde::{Deserialize, Serialize};

#[derive(Debug)]
enum PipelineError {
    Message(String),
}

impl From<io::Error> for PipelineError {
    fn from(e: io::Error) -> PipelineError {
        PipelineError::Message(format!("IO Error: {}", e))
    }
}

#[derive(Serialize, Deserialize)]
struct BakeFileMetadata {
    size: u64,
    modified: time::SystemTime,
}
type BakeCache = HashMap<String, BakeFileMetadata>;

fn get_bake_cache(cache_file_path: &Path) -> Result<BakeCache, PipelineError> {
    let create_empty = || -> Result<BakeCache, PipelineError> {
        File::create(cache_file_path)?;
        Ok(HashMap::new())
    };

    if !cache_file_path.exists() {
        fs::create_dir_all(cache_file_path.parent().unwrap())?;
        return create_empty();
    }

    let cache_file = File::open(cache_file_path)?;
    match bincode::deserialize_from(cache_file) {
        Ok(cache) => Ok(cache),
        Err(_) => {
            fs::remove_file(cache_file_path)?;
            create_empty()
        }
    }
}

fn bake(matches: &ArgMatches) -> Result<(), PipelineError> {
    // Read config file
    let config_path = Path::new(matches.value_of("config").unwrap());
    if !config_path.exists() {
        return Err(PipelineError::Message(format!(
            "Config file does not exists: {}",
            config_path.to_str().unwrap()
        )));
    }

    if !config_path.is_file() {
        return Err(PipelineError::Message(format!(
            "Config argument is not a file: {}",
            config_path.to_str().unwrap()
        )));
    }

    let config_dir = config_path.parent().unwrap();
    let pipeline_dir = PathBuf::from(config_dir).join(".pipeline");

    let cache_file_path = pipeline_dir.join("bake");
    let cache = match get_bake_cache(&cache_file_path) {
        Ok(c) => c,
        Err(e) => {
            return Err(PipelineError::Message(format!(
                "Bake cache file error ({}): {:?}",
                &pipeline_dir.to_str().unwrap(),
                e
            )))
        }
    };

    let raw_config = fs::read_to_string(&config_path)?;
    let lines = raw_config
        .split('\n')
        .map(|line| line.trim().replace("\r", ""))
        .filter(|line| line.starts_with('#') || line.is_empty());

    for line in lines {
        let args: Vec<&str> = line.split_whitespace().collect();

        // if file (parts[0]) not in cache, add it to cache and proceed
        // if file in parts[0] and timestamp changed, proceed
        // if file in parts[0] and size changed, proceed
        // otherwise skip
        match args[0].to_lowercase().as_str() {
            "copy" => {
                // TODO: Copy file
            }
            "stb" => {
                // TODO: Convert CSV to STB
            }
            _ => {} // Skip
        }
    }

    // Create output dir if not exist
    Ok(())
}

fn main() -> Result<(), PipelineError> {
    let mut app = App::new("Rose Next Pipeline Tool").subcommand(
        SubCommand::with_name("bake")
            .about("Rose Next Asset Compiler")
            .arg(Arg::with_name("INPUT_DIR").required(true))
            .arg(
                Arg::with_name("output")
                    .short("o")
                    .help("Output directory")
                    .default_value("../game")
                    .takes_value(true),
            )
            .arg(
                Arg::with_name("config")
                    .short("c")
                    .default_value("bake.manifest")
                    .help("Manifest file")
                    .required(true)
                    .takes_value(true),
            ),
    );
    let mut help = Vec::new();
    app.write_help(&mut help).unwrap();

    let matches = &app.get_matches();

    match matches.subcommand() {
        ("bake", Some(sub_matches)) => bake(sub_matches),
        _ => {
            println!("{}", String::from_utf8(help).unwrap());
            Ok(())
        }
    }
}
