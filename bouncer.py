from threading import *
from Queue import *
import walter
import porter

def main():
    queue = Queue()
    thread_walter = Thread(target=lambda: walter.main(queue))
    thread_porter = Thread(target=lambda: porter.main(queue))

    thread_walter.start()
    thread_porter.start()

if __name__ == '__main__':
    main()
