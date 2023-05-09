
#ifdef BUILD_AS_SHARED
#ifdef DLL_EXPORTS
#define MONKEY_API __declspec(dllexport)
#else
#define MONKEY_API __declspec(dllimport)
#endif
#define CLASS class MONKEY_API
#define STRUCT struct MONKEY_API
#else
#define MONKEY_API

#define CLASS class
#define STRUCT struct
#endif
