import threading
from collections import Counter

from utils import (
    parse_movie_urls,
    parse_movie_annotation,
    split_into_chunks,
    count_words,
)


def run_worker(url, genre, use_stem, num_threads):

    movie_urls = parse_movie_urls(url)

    def process_chunk(urls, result, lock):
        local_counter = Counter()

        for movie_url in urls:
            print(f"Processing film -  {movie_url}")
            text = parse_movie_annotation(movie_url)
            if text:
                local_counter.update(count_words(text, use_stem))

        with lock:
            result.update(local_counter)

    common_counter = Counter()

    lock = threading.Lock()
    chunks = split_into_chunks(movie_urls, num_threads)
    threads = []

    for i, chunk in enumerate(chunks, 1):
        if chunk:
            print(f"Thread {i} started with {len(chunk)} urls")
            t = threading.Thread(target=process_chunk, args=(chunk, common_counter, lock))
            threads.append(t)
            t.start()

    for t in threads:
        t.join()

    with open(f'top_words_{genre}.txt', 'w', encoding='utf-8') as f:
        for word, count in common_counter.most_common(10):
            f.write(f'{word} : {count}\n')


if __name__ == '__main__':
    run_worker(
        url="https://kino.mail.ru/cinema/all/boevik/",
        genre="boevik",
        use_stem=False,
        num_threads=5
    )
