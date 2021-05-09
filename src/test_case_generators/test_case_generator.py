import re
import sys
import random
import subprocess
import time

num_test_cases = 10
max_string_size = 5000
num_regex = 10
max_regex_blocks_size = 5

def giveRandomAlpha():
    capital = random.randint(0, 1)
    if capital:
        return chr(ord('A') + random.randint(0, 25))
    return chr(ord('a') + random.randint(0, 25))

def giveRandomDigit():
    return str(random.randint(0, 9))

def giveRandomAlphaNumeric():
    if random.randint(0, 1) == 1:
        return giveRandomAlpha()
    return giveRandomDigit()

def giveRandomSpecialChar():
    special_char = ['[', '+', '*', '\\', '?']
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
    num_characters = random.randint(1, 5)
    string = '['
    for _ in range(num_characters):
        if random.randint(0, 1) == 1:
            string += str(giveRandomAlphaNumeric())
        else:
            r = random.randint(0, 2)
            if r == 0:
                string += 'a-z'
            elif r == 1:
                string += 'A-Z'
            else:
                string += '0-9'
    string += ']'
    return string

def runTestCase(program_location, input_str, expected_output_str, expected_output_time_taken = None):
    start_time = time.process_time()
    proc = subprocess.Popen(program_location, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    try:
        outs, errs = proc.communicate(input = input_str.encode('utf-8'), timeout=25)
    except TimeoutExpired:
        proc.kill()
        outs, errs = proc.communicate()
    if outs.decode('utf-8') != expected_output_str:
        print("failed:", input_str, "got:", outs.decode('utf-8'), "should be:", expected_output_str)
    else:
        if expected_output_time_taken:
            print("passed! time: program: " + "{:.2f}".format((time.process_time() - start_time) * (10**3)), "ms", "expected_output: " + "{:.2f}".format(expected_output_time_taken * (10**3)), "ms")
        else:
            print("passed! time: program: " + "{:.2f}".format((time.process_time()- start_time) * (10**3)), "ms")

def generateRandomRegex():
    regex = ''
    regex_blocks_size = random.randint(2, max_regex_blocks_size)
    for i in range(regex_blocks_size):
        c = giveRandomCharacter(1)
        block = ''
        if c == '[':
            block = giveRandomClass()
        else:
            if random.randint(0, 1) == 1:
                block = str(giveRandomAlphaNumeric())
            else:
                block = str(giveRandomSpecialChar())
                block = str('\\' + block)
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
        m = re.search(pat, string)
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

for _ in range(num_test_cases):
    string = ""
    for _ in range(max_string_size):
        string += str(giveRandomCharacter())
    input_str = ''
    input_str += string
    input_str += '\n'
    input_str += str(num_regex)
    input_str += '\n'

    for _ in range(num_regex):
        input_str += str(generateRandomRegex())
        input_str += '\n'

    start_time = time.process_time()
    expected_output_str = pyRegexRun(input_str)
    end_time = time.process_time()

    runTestCase('./v2', input_str, expected_output_str, end_time - start_time)
