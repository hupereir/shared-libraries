#ifndef base_help_export_h
#define base_help_export_h

#if defined (_WIN32)
  #ifdef base_help_EXPORTS
    /* We are building this library */
    #define BASE_HELP_EXPORT Q_DECL_EXPORT
  #else
    /* We are using this library */
    #define BASE_HELP_EXPORT Q_DECL_IMPORT
  #endif
#else
  #define BASE_HELP_EXPORT
#endif

#endif
