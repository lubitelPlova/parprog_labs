import requests
from bs4 import BeautifulSoup
from stop_words import get_stop_words
from nltk.stem import SnowballStemmer
import re
from collections import Counter
import time
import random


STOP_WORDS = get_stop_words('russian')
HEADERS = {'User-Agent': 'Mozilla/5.0'}


def parse_movie_urls(url):
    time.sleep(random.uniform(5, 10))

    response = requests.get(url, headers=HEADERS)
    # response.raise_for_status()
    soup = BeautifulSoup(response.text, 'html.parser')

    links = soup.select('a.link-holder_itemevent_small')

    movie_urls = []
    for link in links:
        href = link.get('href')
        movie_urls.append(f'https://kino.mail.ru{href}')

    return movie_urls


def parse_movie_annotation(movie_url):
    max_retries = 5

    for attempt in range(max_retries):
        time.sleep(random.uniform(5+2*attempt, 13+2*attempt))

        response = requests.get(movie_url, HEADERS)

        if response.status_code == 200:
            soup = BeautifulSoup(response.text, 'html.parser')
            annotations = soup.find('p')

            if annotations:
                return annotations.text.strip()
            print(f'Annotation not found on page (try {attempt+1}/{max_retries})')
            continue

        print(f"Unawaited response code: {response.status_code} (try {attempt+1}/{max_retries})")

    raise RuntimeError(f"Annotation parsing failed after {max_retries} tries: {movie_url}")


def count_words(text, use_stem):
    stemmer = SnowballStemmer('russian')

    words = re.findall(r'\b[а-яёa-z]+\b', text.lower())

    clean_words = [stemmer.stem(w) if use_stem else w for w in words if w not in STOP_WORDS]

    return Counter(clean_words)


def split_into_chunks(lst, n):
    if n <= 0:
        return [lst]
    k, m = divmod(len(lst), n)
    return [lst[i*k + min(i, m):(i+1)*k + min(i+1, m)] for i in range(n)]
