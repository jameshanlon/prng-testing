#!/usr/bin/env python

from multiprocessing import Pool
import os
import subprocess
import sys
import shutil

NUM_SEEDS = 100


def run_test(index):
    seed = 1 + index * ((2**128) / 100)
    seed_lo64 = seed & ((1 << 64) - 1);
    seed_hi64 = seed >> 64 & ((1 << 64) - 1);
    print 'Starting job {} with seed: {}'.format(index, seed)
    try:
        cmd = [test_exe, str(seed_lo64), str(seed_hi64)]
        output = subprocess.check_output(cmd)
    except subprocess.CalledProcessError as e:
        print e.output
    filename = results_dir+'/'+str(seed)+'.txt'
    f = open(filename, 'w')
    f.write(output)
    f.close()


if __name__ == '__main__':
    if (len(sys.argv) != 3):
        print 'USAGE: {} [path/to/test-exe] [results-dir-name]'.format(sys.argv[0])
        sys.exit(1)
    global test_exe
    global results_dir
    test_exe = os.path.abspath(sys.argv[1])
    results_dir = os.path.join(os.getcwd(), sys.argv[2])
    if not os.path.exists(test_exe):
        print "ERROR: test exe '"+test_exe+"' does not exist."
        sys.exit(1)
    if os.path.exists(results_dir):
        print 'Removing old results directory: '+results_dir
        shutil.rmtree(results_dir)
    os.makedirs(results_dir)
    p = Pool()
    p.map(run_test, range(NUM_SEEDS))
