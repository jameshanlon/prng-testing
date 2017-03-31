#!/usr/bin/env python

import sys
import re
import os

if __name__ == '__main__':
    if (len(sys.argv) != 2):
        print 'USAGE: {} [results-dir-name]'.format(sys.argv[0])
        sys.exit(1)

    directory=sys.argv[1]
    total_failures = 0

    for filename in os.listdir(directory):
        if not filename.endswith('.txt'):
            continue
        f = open(os.path.join(directory, filename), 'r')
        contents = f.read()
        failures = []
        if not 'All tests were passed' in contents:
            match = re.search(r'=+ Summary results of [A-Za-z]+ =+', contents)
            contents = contents[match.start():]
            match = re.search(r'-+\n', contents)
            contents = contents[match.end():]
            match = re.search(r'-+\n', contents)
            contents = contents[:match.start()]
            for failure in contents.split('\n'):
                if failure.strip():
                    failures.append(failure.split())
            f.close()
        total_failures += len(failures)
        failures_list = ', '.join([x[1] for x in failures])
        failures_desc = '{} failure(s): {}'.format(len(failures), failures_list)
        print '{} {}'.format(filename, failures_desc if len(failures) else '')
    print '{} total failure(s)'.format(total_failures)
