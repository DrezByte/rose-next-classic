use clap::{crate_version, App, Arg, SubCommand};

pub mod bake;
pub mod error;
pub mod pack;

use crate::bake::bake;
use crate::error::PipelineError;

fn main() -> Result<(), PipelineError> {
    let app = App::new("Rose Next Pipeline Tool")
        .version(crate_version!())
        .subcommand(
            SubCommand::with_name("bake")
                .about("Rose Next Asset Compiler")
                .arg(Arg::with_name(bake::BAKE_INPUT_DIR).required(true))
                .arg(Arg::with_name(bake::BAKE_OUTPUT_DIR).required(true))
                .arg(
                    Arg::with_name(bake::BAKE_CONFIG_NAME)
                        .short("c")
                        .default_value("bake.manifest")
                        .help("Name of the manifest file in the input directory")
                        .required(true)
                        .takes_value(true),
                )
                .arg(
                    Arg::with_name(bake::BAKE_CLEAN_NAME)
                        .long(bake::BAKE_CLEAN_NAME)
                        .help("Removes any invalid files from output directory")
                        .takes_value(false),
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
