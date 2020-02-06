#ifndef base_filesystem_export_h
#define base_filesystem_export_h

#if defined (_WIN32)
  #include <QtCore/QtGlobal>
  #ifdef base_filesystem_EXPORTS
    /* We are building this library */
    #define BASE_FILESYSTEM_EXPORT Q_DECL_EXPORT
  #else
    /* We are using this library */
    #define BASE_FILESYSTEM_EXPORT Q_DECL_IMPORT
  #endif
#else
  #define BASE_FILESYSTEM_EXPORT
#endif

#endif
