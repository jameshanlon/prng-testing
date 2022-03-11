import os
import sys
import argparse
import run_tests

DEFAULT_NUM_SEEDS = 100
DEFAULT_MP_POOL_SIZE = 128
TESTU01_OUTPUTS = ['std32', 'rev32', 'std32lo', 'rev32lo', 'std32hi', 'rev32hi']

def main():
    parser = argparse.ArgumentParser(description='Run all test sets against a PRNG')
    parser.add_argument('generator',
                        help='Specify the generator binary')
    parser.add_argument('--numseeds',
                        type=int,
                        default=DEFAULT_NUM_SEEDS,
                        help='The number of seeds to test')
    parser.add_argument('--dryrun',
                        action='store_true',
                        help='Don\'t run anything')
    parser.add_argument('--mppoolsize',
                        type=int,
                        default=DEFAULT_MP_POOL_SIZE,
                        help='The size of the multiprocessing pool')
    parser.add_argument('--smallcrush',
                        action='store_true',
                        help='Run SmallCrush')
    parser.add_argument('--crush',
                        action='store_true',
                        help='Run Crush')
    parser.add_argument('--bigcrush',
                        action='store_true',
                        help='Run BigCrush')
    parser.add_argument('--practrand',
                        action='store_true',
                        help='Run Practrand')
    parser.add_argument('--gjrand',
                        action='store_true',
                        help='Run Gjrand')
    args = parser.parse_args()
    generator_name = os.path.basename(args.generator)
    # SmallCrush
    if args.smallcrush:
        for output in TESTU01_OUTPUTS:
            run_tests.run_all_seeds(args.generator,
                                    'smallcrush',
                                    output,
                                    args.numseeds,
                                    f'TEST_{generator_name}_smallcrush_{output}_{args.numseeds}',
                                    args.dryrun,
                                    args.mppoolsize)
    # Crush
    if args.crush:
        for output in TESTU01_OUTPUTS:
            run_tests.run_all_seeds(args.generator,
                                    'crush',
                                    output,
                                    args.numseeds,
                                    f'TEST_{generator_name}_crush_{output}_{args.numseeds}',
                                    args.dryrun,
                                    args.mppoolsize)
    # BigCrush
    if args.bigcrush:
        for output in TESTU01_OUTPUTS:
            run_tests.run_all_seeds(args.generator,
                                    'bigcrush',
                                    output,
                                    args.numseeds,
                                    f'TEST_{generator_name}_bigcrush_{output}_{args.numseeds}',
                                    args.dryrun,
                                    args.mppoolsize)
    # PractRand
    if args.practrand:
        run_tests.run_all_seeds(args.generator,
                                'practrand',
                                'std64',
                                args.numseeds,
                                f'TEST_{generator_name}_practrand_{args.numseeds}',
                                args.dryrun,
                                args.mppoolsize)
    # Gjrand
    if args.gjrand:
        run_tests.run_all_seeds(args.generator,
                                'gjrand',
                                'std64',
                                args.numseeds,
                                f'TEST_{generator_name}_gjrand_{args.numseeds}',
                                args.dryrun,
                                args.mppoolsize)
    return 0


if __name__ == '__main__':
  sys.exit(main())
