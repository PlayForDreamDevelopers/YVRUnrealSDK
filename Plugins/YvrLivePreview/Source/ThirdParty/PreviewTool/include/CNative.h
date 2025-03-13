#ifndef PREVIEWTOOL_CNATIVE_H
#define PREVIEWTOOL_CNATIVE_H


#if C_API_EXPORT
#define C_API extern "C" __declspec(dllexport)
#else
#define C_API extern "C" __declspec(dllimport)
#endif

typedef void (*on_state_changed)(int state);

typedef struct {
    on_state_changed state_changed;
} native_event_t;

C_API void native_start();
C_API void native_stop();
C_API void native_set_event_listener(native_event_t *event);
C_API void native_set_display_resolution(int width, int height);
C_API void native_set_delay_time(int time);

#endif //PREVIEWTOOL_CNATIVE_H
