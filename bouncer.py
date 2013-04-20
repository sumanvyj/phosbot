from threading import *
from Queue import *
import sys
import time
import walter
import porter

def main(phrase=None):
    queue = Queue()
    walter_t = Thread(target=lambda: walter.main(phrase=phrase, queue=queue))
    porter_t = Thread(target=lambda: porter.main(queue=queue))

    walter_t.daemon = True
    porter_t.daemon = True

    walter_t.start()
    porter_t.start()

    try:
        while True:
            time.sleep(0.01)
    except (KeyboardInterrupt, SystemExit):
        return

if __name__ == '__main__':
    if len(sys.argv) > 1:
        main(sys.argv[1])
    else:
        main()
