// Detect Windows OS
#ifdef _WIN32
#    define SS_EXP __declspec(dllexport)
#    define SS_IMP __declspec(dllimport)
// Unix-based - Linux and OS X
#elif __unix__
#    define SS_EXP __attribute__((visibility("default")))
#    define SS_IMP __attribute__((visibility("default")))
#else
#    error "Unknown_compiler"
#endif
