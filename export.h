#ifndef __EXPORT_H__
#define __EXPORT_H__

// Detect Windows OS
#if defined(__WIN32__) || defined(_WIN32)
#  ifdef GLOBAL_DEF
#    define EXP_DEFINES __declspec(dllexport)
#  else
#    define EXP_DEFINES __declspec(dllimport)
#  endif
#  ifdef PLUGIN
#    define SS_EXP __declspec(dllexport)
#  else
#    define SS_EXP __declspec(dllimport)
#  endif
// Unix-based - Linux and OS X
#elif __unix__ || __APPLE__
#  define SS_EXP __attribute__((visibility("default")))
#  define EXP_DEFINES __attribute__((visibility("default")))
#else
#    error "Unknown_compiler"
#endif

#endif // __EXPORT_H__
