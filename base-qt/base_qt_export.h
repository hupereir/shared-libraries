#ifndef base_qt_export_h
#define base_qt_export_h

#if defined (_WIN32)
  #include <QtCore/QtGlobal>
  #ifdef base_qt_EXPORTS
    /* We are building this library */
    #define BASE_QT_EXPORT Q_DECL_EXPORT
  #else
    /* We are using this library */
    #define BASE_QT_EXPORT Q_DECL_IMPORT
  #endif
#else
  #define BASE_QT_EXPORT
#endif

#endif
