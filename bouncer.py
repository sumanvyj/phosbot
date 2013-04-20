from threading import *
from Queue import *
import time
import walter
import porter

def main():
    queue = Queue()
    walter_t = Thread(target=lambda: walter.main(None, queue))
    porter_t = Thread(target=lambda: porter.main(queue))

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
    main()
