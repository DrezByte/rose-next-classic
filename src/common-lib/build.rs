use cbindgen;

fn main() {
  let crate_dir = env!("CARGO_MANIFEST_DIR");
  let config = cbindgen::Config::from_root_or_default(&crate_dir);

  cbindgen::generate_with_config(&crate_dir, config)
    .expect("Failed to generate bindings")
    .write_to_file("include/rose/common/common_interface.h");
}
