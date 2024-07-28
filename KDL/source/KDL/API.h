
#ifndef KDL_API_H
#define KDL_API_H

#ifdef KDL_STATIC_DEFINE
#  define KDL_API
#  define KDL_NO_EXPORT
#else
#  ifndef KDL_API
#    ifdef KDL_EXPORTS
        /* We are building this library */
#      define KDL_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define KDL_API __declspec(dllimport)
#    endif
#  endif

#  ifndef KDL_NO_EXPORT
#    define KDL_NO_EXPORT 
#  endif
#endif

#ifndef KDL_DEPRECATED
#  define KDL_DEPRECATED __declspec(deprecated)
#endif

#ifndef KDL_DEPRECATED_EXPORT
#  define KDL_DEPRECATED_EXPORT KDL_API KDL_DEPRECATED
#endif

#ifndef KDL_DEPRECATED_NO_EXPORT
#  define KDL_DEPRECATED_NO_EXPORT KDL_NO_EXPORT KDL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef KDL_NO_DEPRECATED
#    define KDL_NO_DEPRECATED
#  endif
#endif

#endif /* KDL_API_H */
