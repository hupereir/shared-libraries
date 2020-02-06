#ifndef base_svg_export_h
#define base_svg_export_h

#if defined (_WIN32)
  #ifdef base_svg_EXPORTS
    /* We are building this library */
    #define BASE_SVG_EXPORT Q_DECL_EXPORT
  #else
    /* We are using this library */
    #define BASE_SVG_EXPORT Q_DECL_IMPORT
  #endif
#else
  #define BASE_SVG_EXPORT
#endif

#endif
