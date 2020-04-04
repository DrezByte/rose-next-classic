use std::collections::HashMap;
use std::error::Error;
use std::fs;
use std::fs::File;
use std::io;
use std::path::{Path, PathBuf};
use std::time;

use bincode;
use clap::{App, Arg, ArgMatches, SubCommand};
use globset;
use globset::{Glob, GlobSetBuilder};
use serde::{Deserialize, Serialize};
use walkdir::WalkDir;

const BAKE_INPUT_DIR: &str = "INPUT_DIR";
const BAKE_OUTPUT_DIR: &str = "OUTOUT_DIR";
const BAKE_CONFIG_NAME: &str = "config_name";

#[derive(Debug)]
enum PipelineError {
    Message(String),
}

impl From<io::Error> for PipelineError {
    fn from(e: io::Error) -> PipelineError {
        PipelineError::Message(format!("IO Error: {}", e))
    }
}

impl From<globset::Error> for PipelineError {
    fn from(e: globset::Error) -> PipelineError {
        PipelineError::Message(format!("Glob Error: {}", e))
    }
}

#[derive(Serialize, Deserialize, Clone, Copy, Debug, PartialEq)]
struct BakeFileMetadata {
    size: u64,
    modified: time::SystemTime,
}

impl Default for BakeFileMetadata {
    fn default() -> BakeFileMetadata {
        BakeFileMetadata {
            size: 0,
            modified: time::SystemTime::now(),
        }
    }
}

impl From<&fs::Metadata> for BakeFileMetadata {
    fn from(m: &fs::Metadata) -> BakeFileMetadata {
        BakeFileMetadata {
            size: m.len(),
            modified: m.modified().unwrap_or(time::SystemTime::now()),
        }
    }
}

type BakeCache = HashMap<PathBuf, BakeFileMetadata>;

/// Read the bake file metadata cache from disk
fn get_bake_cache(cache_file_path: &Path) -> Result<BakeCache, PipelineError> {
    if !cache_file_path.exists() {
        return Ok(HashMap::new());
    }

    let cache_file = File::open(cache_file_path)?;
    match bincode::deserialize_from(cache_file) {
        Ok(cache) => Ok(cache),
        Err(_) => Ok(HashMap::new()),
    }
}

/// Save the bake file metadata cache to disk
fn save_bake_cache(cache_file_path: &Path, cache: &BakeCache) -> Result<(), PipelineError> {
    if !cache_file_path.exists() {
        fs::create_dir_all(cache_file_path.parent().unwrap())?;
    }

    let cache_file = File::create(cache_file_path)?;
    if let Err(e) = bincode::serialize_into(cache_file, cache) {
        return Err(PipelineError::Message(format!(
            "Failed to save cache file: {}",
            e
        )));
    }

    Ok(())
}

/// Checks if the path needs to be re-baked by comparing current file data
/// to the value stored in the cache. Updates the cache with the new metadata.
///
/// Returns true if:
/// - `path` does not exist
/// - `path` is not in the cache
/// - `metadata` does not match the metadata in `cache`
fn should_rebake(path: &Path, metadata: &BakeFileMetadata, cache: &mut BakeCache) -> bool {
    if !path.exists() {
        return false;
    }

    let metadata = *metadata;
    if let Some(cached_metadata) = cache.insert(path.to_path_buf(), metadata) {
        cached_metadata != metadata
    } else {
        true
    }
}

fn bake(matches: &ArgMatches) -> Result<(), PipelineError> {
    let config_name = matches.value_of(BAKE_CONFIG_NAME).unwrap();

    let input_dir = PathBuf::from(matches.value_of(BAKE_INPUT_DIR).unwrap());
    if !input_dir.exists() {
        return Err(PipelineError::Message(format!(
            "Invalid input dir: {}",
            input_dir.to_str().unwrap()
        )));
    }

    let config_path = input_dir.join(config_name);
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

    let pipeline_dir = input_dir.join(".pipeline");
    let cache_file_path = pipeline_dir.join("bake");

    let mut cache = match get_bake_cache(&cache_file_path) {
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
        .filter(|line| !line.starts_with('#') && !line.is_empty());

    let output_dir = PathBuf::from(matches.value_of(BAKE_OUTPUT_DIR).unwrap());
    if !output_dir.exists() {
        fs::create_dir_all(&output_dir)?;
    }

    let walk_dir = match fs::metadata(&input_dir) {
        Ok(m) => should_rebake(&input_dir, &BakeFileMetadata::from(&m), &mut cache),
        Err(_) => true,
    };

    let mut file_list = Vec::new();
    let mut file_metadata: HashMap<PathBuf, BakeFileMetadata> = HashMap::new();

    if walk_dir {
        for entry in WalkDir::new(&input_dir).into_iter() {
            if let Ok(ent) = entry {
                let entry_path = ent.into_path();
                if !entry_path.is_file() {
                    continue;
                }

                let canonical_path = entry_path.canonicalize().unwrap();
                file_list.push(canonical_path.clone());

                match fs::metadata(&entry_path) {
                    Ok(m) => {
                        file_metadata.insert(canonical_path, BakeFileMetadata::from(&m));
                    }
                    Err(e) => {
                        println!(
                            "Error reading metadata for file {}: {}",
                            entry_path.display(),
                            e
                        );
                        continue;
                    }
                }
            }
        }
    }

    let mut glob_builder = GlobSetBuilder::new();
    let mut commands: Vec<Vec<String>> = Vec::new();

    for (line_number, line) in lines.enumerate() {
        let line_number = line_number + 1;

        let args: Vec<String> = line.split_whitespace().map(|s| s.to_string()).collect();
        if args.len() < 2 {
            continue;
        }

        let command = args[0].to_lowercase();
        let file_glob = &args[1];

        match Glob::new(&file_glob) {
            Ok(g) => {
                glob_builder.add(g);
                commands.push(args);
            }
            Err(e) => {
                println!("Skipping line {}: Error: {}", line_number, e);
            }
        }
    }

    let globs = glob_builder.build()?;

    for filepath in file_list {
        let rebake = match fs::metadata(&filepath) {
            Ok(m) => should_rebake(&filepath, &BakeFileMetadata::from(&m), &mut cache),
            Err(e) => {
                println!("Error reading file metadata {}: {}", filepath.display(), e);
                false
            }
        };

        if !rebake {
            continue;
        }

        let command_indices = globs.matches(filepath);
        for command_index in command_indices.iter() {
            let command_index = *command_index;
            let args = &commands[command_index];

            let command = args[0].to_lowercase();
            match command.as_str() {
                "copy" => {}
                "stb" => {}
                "dds" => {}
                _ => {} // Unrecognized command
            }
        }
    }

    let _ = save_bake_cache(cache_file_path.as_path(), &cache);

    Ok(())
}

fn main() -> Result<(), PipelineError> {
    let app = App::new("Rose Next Pipeline Tool").subcommand(
        SubCommand::with_name("bake")
            .about("Rose Next Asset Compiler")
            .arg(Arg::with_name(BAKE_INPUT_DIR).required(true))
            .arg(Arg::with_name(BAKE_OUTPUT_DIR).required(true))
            .arg(
                Arg::with_name(BAKE_CONFIG_NAME)
                    .short("c")
                    .default_value("bake.manifest")
                    .help("Name of the manifest file in the input directory")
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
