#!/bin/env python3
"""A python script to automatically download the puzzle inputs and 
example inputs for any day.

The `COOKIE` variable correspondes to my account should be changed
accordingly since the inputs are different for each user. Use the
inspect window in your browser and locate the session cookie to
change it.
"""
import os
import re
import json
import argparse
import datetime
import requests
from bs4 import BeautifulSoup


with open('cookie.json', 'r') as file:
    COOKIE = json.load(file)


def main() -> int:
    """The main function of the script.

    Returns
    -------
    int
        Exit code
    """
    parser = argparse.ArgumentParser()
    parser.add_argument("day", type=int, help="The day of the puzzle input to download")

    year = datetime.date.today().year

    args = parser.parse_args()
    if args.day <= 0:
        print("Error: Day should be a positive number")
        return 1

    filename = f"data/{args.day:02d}.txt"
    if os.path.exists(filename):
        print(f'Input file "{filename}" already exists.')
        return 1

    url = f"https://adventofcode.com/{year}/day/{args.day}/input"
    request = requests.get(url, cookies=COOKIE)
    if not request.ok:
        if request.status_code == 404:
            print(f"Puzzle for day {args.day} has not unlock yet.")
            return 1

        print(f"Unknown Error {request.status_code} with text: {request.text}")
        return 1

    with open(filename, "w") as file:
        file.write(request.text)

    example_fname = f"data/examples/{args.day:02d}.txt"
    if os.path.exists(example_fname):
        print(f'Example data file "{example_fname}" already exists')
        return 0

    url = f"https://adventofcode.com/{year}/day/{args.day}"
    request = requests.get(url, cookies=COOKIE)
    soup = BeautifulSoup(request.text, "html.parser")

    example_p = soup.find(string=re.compile("For example|[Ll]arger example|[Hh]ere is an example"))
    example_data = example_p.findNext("code").text

    with open(example_fname, "w") as file:
        file.write(example_data)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
