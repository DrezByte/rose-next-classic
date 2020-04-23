use std::ffi::{CStr, CString};
use std::fmt;
use std::ptr;

#[repr(C)]
#[derive(Debug)]
pub struct FfiString {
    chars: *mut libc::c_char,
    len: libc::size_t,
}

impl FfiString {
    pub fn new() -> FfiString {
        Self::default()
    }

    pub fn set_string(&mut self, v: &str) {
        let c_str = CString::new(v.as_bytes()).unwrap_or_default();
        self.len = c_str.as_bytes().len();
        self.chars = c_str.into_raw() as *mut libc::c_char;
    }
}

impl Default for FfiString {
    fn default() -> FfiString {
        FfiString {
            chars: ptr::null::<libc::c_char>() as *mut libc::c_char,
            len: 0,
        }
    }
}

impl Drop for FfiString {
    fn drop(&mut self) {
        unsafe {
            CString::from_raw(self.chars); // Drop
        }
    }
}

impl fmt::Display for FfiString {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let s = unsafe { CStr::from_ptr(self.chars).to_string_lossy().to_string() };
        write!(f, "{}", s)
    }
}

#[no_mangle]
pub unsafe extern "C" fn ffi_string_new() -> *mut FfiString {
    Box::into_raw(Box::new(FfiString::new()))
}

#[no_mangle]
pub unsafe extern "C" fn ffi_string_free(s: *mut FfiString) {
    Box::from_raw(s); // Drop
}
