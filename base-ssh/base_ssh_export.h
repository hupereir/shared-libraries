#ifndef base_ssh_export_h
#define base_ssh_export_h

#if defined (_WIN32)
  #ifdef base_ssh_EXPORTS
    /* We are building this library */
    #define BASE_SSH_EXPORT Q_DECL_EXPORT
  #else
    /* We are using this library */
    #define BASE_SSH_EXPORT Q_DECL_IMPORT
  #endif
#else
  #define BASE_SSH_EXPORT
#endif

#endif
