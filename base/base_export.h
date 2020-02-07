#ifndef base_export_h
#define base_export_h

#if defined _WIN32 && !defined STATIC_BUILD
  #include <QtCore/QtGlobal>
  #ifdef base_EXPORTS
    /* We are building this library */
    #define BASE_EXPORT Q_DECL_EXPORT
  #else
    /* We are using this library */
    #define BASE_EXPORT Q_DECL_IMPORT
  #endif
#else
  #define BASE_EXPORT
#endif

#endif
