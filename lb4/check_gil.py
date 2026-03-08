import sys
if hasattr(sys, '_is_gil_enabled'):
    if sys._is_gil_enabled():
        print("GIL is ENABLED")
    else:
        print("GIL is DISABLED (free-threaded)")
