# main.py
import argparse
from multiprocessing import Pool
from worker import run_worker
import time


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--num-threads', type=int, default=5)
    parser.add_argument('--use-stem', action='store_true')
    return parser.parse_args()


TASKS = [
    ("https://kino.mail.ru/cinema/all/boevik/  ", "boevik"),
    ("https://kino.mail.ru/cinema/all/drama/  ", "drama"),
    ("https://kino.mail.ru/cinema/all/komedija/  ", "komedija"),
]


def run_task(args):
    url, genre, use_stem, num_threads = args
    return run_worker(url, genre, use_stem, num_threads)


if __name__ == '__main__':
    start_time = time.time()

    args = parse_args()

    TASKS_WITH_ARGS = [
        (url, genre, args.use_stem, args.num_threads)
        for url, genre in TASKS
    ]

    with Pool(processes=len(TASKS), maxtasksperchild=1) as pool:
        results = pool.map(run_task, TASKS_WITH_ARGS)
        
    elapsed_time = time.time() - start_time
    print(f"\nEnded!\n\tTime elapsed: {elapsed_time:.2f} sec")
