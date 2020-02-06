#ifndef base_spellcheck_export_h
#define base_spellcheck_export_h

#if defined (_WIN32)
  #include <QtCore/QtGlobal>
  #ifdef base_spellcheck_EXPORTS
    /* We are building this library */
    #define BASE_SPELLCHECK_EXPORT Q_DECL_EXPORT
  #else
    /* We are using this library */
    #define BASE_SPELLCHECK_EXPORT Q_DECL_IMPORT
  #endif
#else
  #define BASE_SPELLCHECK_EXPORT
#endif

#endif
