#ifndef base_notifications_export_h
#define base_notifications_export_h

#if defined _WIN32 && !defined STATIC_BUILD
  #include <QtCore/QtGlobal>
  #ifdef base_notifications_EXPORTS
    /* We are building this library */
    #define BASE_NOTIFICATIONS_EXPORT Q_DECL_EXPORT
  #else
    /* We are using this library */
    #define BASE_NOTIFICATIONS_EXPORT Q_DECL_IMPORT
  #endif
#else
  #define BASE_NOTIFICATIONS_EXPORT
#endif

#endif
