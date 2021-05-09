#!/usr/bin/python3.7

import re
import sys
import random
import subprocess
import time
import traceback

num_test_cases = 1000
max_string_size = 4001
num_regex = 100
max_regex_blocks_size = 5

def randint(start : int, stop : int):
    return int((stop - start + 1) * random.random()) + start

def giveRandomAlpha():
    capital = randint(0, 1)
    if capital:
        return chr(ord('A') + randint(0, 25))
    return chr(ord('a') + randint(0, 25))

def giveRandomDigit():
    return str(randint(0, 9))

def giveRandomAlphaNumeric():
    if randint(0, 1) == 1:
        return giveRandomAlpha()
    return giveRandomDigit()

def giveRandomSpecialChar():
    special_char = ['[', '+', '*', '\\', '?', '.']
    return random.choice(special_char)

def giveRandomCharacter(some_value = 50, no_square = False):
    l = []
    for _ in range(some_value):
        l.append(giveRandomAlpha())
    for _ in range(some_value):
        l.append(giveRandomDigit())
    special_char = giveRandomSpecialChar()
    if no_square:
        while (special_char != '['):
            special_char = giveRandomSpecialChar()
    l.append(special_char)
    return random.choice(l)

def giveRandomClass():
    num_characters = randint(1, 5)
    string = '['
    for _ in range(num_characters):
        if randint(0, 1) == 1:
            string += str(giveRandomAlphaNumeric())
        else:
            r = randint(0, 2)
            if r == 0:
                string += 'a-z'
            elif r == 1:
                string += 'A-Z'
            else:
                string += '0-9'
    string += ']'
    return string

def runTestCase(program_location, input_str, expected_output_str, expected_output_time_taken = None):
    start_time = time.perf_counter_ns()
    proc = subprocess.Popen(program_location, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    try:
        outs, errs = proc.communicate(input = input_str.encode('utf-8'), timeout=25)
    except TimeoutExpired:
        proc.kill()
        outs, errs = proc.communicate()
    if outs.decode('utf-8') != expected_output_str:
        print("failed:", input_str, "got:", outs.decode('utf-8'), "should be:", expected_output_str, sep='\n')
    else:
        if expected_output_time_taken:
            print("passed! time: program: " + "{:.2f}".format((time.perf_counter_ns() - start_time) / (10**6)), "ms", "expected_output: " + "{:.2f}".format(expected_output_time_taken / (10**6)), "ms")
        else:
            print("passed! time: program: " + "{:.2f}".format((time.perf_counter_ns() - start_time) / (10**6)), "ms")

def generateRandomRegex():
    regex = ''
    regex_blocks_size = randint(2, max_regex_blocks_size)
    special_char = ['[', '+', '*', '\\', '?', '.']
    regex += str(giveRandomAlphaNumeric())
    previous_was_special = False
    for i in range(regex_blocks_size - 1):
        c = giveRandomCharacter(2)
        block = ''
        if c == '[':
            block = giveRandomClass()
            previous_was_special = False
        elif c in special_char and c != '\\' and (not previous_was_special):
            block = str(c)
            if c == '*':
                if randint(0, 4) == 0:
                    block += '?'
            elif c == '.':
                if randint(0, 4) == 0:
                    block += '*'
            previous_was_special = True
        else:
            if randint(0, 10) == 0:
                block = str(giveRandomSpecialChar())
                block = str('\\' + block)
                previous_was_special = False
            elif randint(0, 3) == 0:
                if randint(0, 1) == 0:
                    block = '\d'
                else:
                    block = '\w'
                if randint(0, 2) == 0:
                    block += '*'
                    previous_was_special = True
                else:
                    previous_was_special = False
            else:
                block = str(giveRandomAlphaNumeric())
                previous_was_special = False
        regex += block
    return regex

def pyRegexRun(input_str):
    lines = input_str.split('\n')
    string = lines[0]
    num = int(lines[1])
    lines = lines[2:]
    out_str = ''
    # print(string)
    # print(num)
    for pat in lines:
        if pat == '':
            continue
        # print(pat)
        try:
            m = re.search(pat, string)
        except:
            print(string)
            print(pat)
            print(traceback.print_exc())
            exit(-1)
        if m:
            start = m.start(0)
            end = m.end(0) - 1
            if start < 0:
                start = 0
            if end < 0:
                end = 0
            out_str += '1 '
            out_str += str(start)
            out_str += ' '
            out_str += str(end)
            out_str += '\n'
        else:
            out_str += '0\n'
    return out_str

from itertools import islice
import collections

def consume(iterator, n):
    "Advance the iterator n-steps ahead. If n is none, consume entirely."
    # Use functions that consume iterators at C speed.
    if n is None:
        # feed the entire iterator into a zero-length deque
        collections.deque(iterator, maxlen=0)
    else:
        # advance to the empty slice starting at position n
        next(islice(iterator, n, n), None)

def generateRandomString():
    string = ""
    string_range = iter(range(max_string_size))
    for _ in string_range:
        if randint(0, 20) == 0:
            num_repeat = randint(1, 12)
        else:
            num_repeat = 1
        c = str(giveRandomCharacter())
        for _ in range(num_repeat):
            if len(string) >= max_string_size:
                continue
            string += c
        consume(string_range, num_repeat - 1)
    return string

def generateAndRun():
    for _ in range(num_test_cases):
        string = str(generateRandomString())
        input_str = ''
        input_str += string
        input_str += '\n'
        input_str += str(num_regex)
        input_str += '\n'

        for _ in range(num_regex):
            input_str += str(generateRandomRegex())
            input_str += '\n'

        start_time = time.perf_counter_ns()
        expected_output_str = pyRegexRun(input_str)
        # print(expected_output_str)
        end_time = time.perf_counter_ns()

        runTestCase('./a.out', input_str, expected_output_str, end_time - start_time)

# for _ in range(100):
#     print(generateRandomRegex())
# for _ in range(10):
#     print(generateRandomString())
generateAndRun()
