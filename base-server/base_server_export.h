#ifndef base_server_export_h
#define base_server_export_h

#if defined _WIN32 && !defined STATIC_BUILD
  #include <QtCore/QtGlobal>
  #ifdef base_server_EXPORTS
    /* We are building this library */
    #define BASE_SERVER_EXPORT Q_DECL_EXPORT
  #else
    /* We are using this library */
    #define BASE_SERVER_EXPORT Q_DECL_IMPORT
  #endif
#else
  #define BASE_SERVER_EXPORT
#endif

#endif
