import signal
import os
import time

time.sleep(5)

os.kill(os.getppid(), signal.SIGPIPE)