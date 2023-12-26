import os
import sys
import shutil
import subprocess
from itertools import repeat
from multiprocessing import Pool
import argparse


DEFAULT_MP_POOL_SIZE = 128
DEFAULT_NUM_SEEDS = 100
ROOT_DIR = os.getcwd()

gjrand_exe = os.path.join(ROOT_DIR, "install", "gjrand.4.3.0.0", "testunif", "pmcp")
gjrand_bin = os.path.join(ROOT_DIR, "install", "gjrand.4.3.0.0", "testunif", "bin")

# Setup paths to generator and test binaries
custom_env = os.environ.copy()
custom_env["PATH"] = ROOT_DIR + ":" + custom_env["PATH"]
custom_env["PATH"] = os.path.join(ROOT_DIR, "build") + ":" + custom_env["PATH"]
custom_env["PATH"] = (
    os.path.join(ROOT_DIR, "install", "PractRand-pre0.95") + ":" + custom_env["PATH"]
)


class TestU01Arguments:
    def __init__(self, generator, testsuite, output, output_shift):
        self.generator = generator
        self.testsuite = testsuite
        self.output = output
        self.output_shift = output_shift

    def get_args(self, s0, s1):
        return [
            self.generator,
            self.testsuite,
            self.output,
            s0,
            s1,
            "--output-shift",
            str(self.output_shift),
        ]


class CmdLineArguments:
    def __init__(self, generator, output, extra_args):
        self.generator = generator
        self.output = output
        self.extra_args = extra_args

    def get_args(self, s0, s1):
        return [
            "bash",
            "-c",
            f'"{self.generator} stdout {self.output} {s0} {s1} | {self.extra_args}"',
        ]


def run_command(cmd, working_directory, dry_run):
    name = working_directory.replace("/", "_")
    print("Running: '{}' in {}".format((" ".join(cmd)), working_directory))
    try:
        if not dry_run:
            subprocess.check_call(cmd, cwd=working_directory, env=custom_env)
    except subprocess.CalledProcessError as e:
        print(e.output)


def run_test(index, testsuite, num_seeds, gen_args, output_directory, dry_run):
    """
    Run a single test with an equidistantly-spaced seed at 'index'.
    """
    seed = int(1 + index * ((2**128) // num_seeds))
    seed_lo64 = seed & int((1 << 64) - 1)
    seed_hi64 = int(seed >> 64) & int((1 << 64) - 1)
    print("Starting job with seed: {}".format(hex(seed)))
    working_directory = os.path.join(output_directory, "seed_{}".format(seed))
    if not os.path.exists(working_directory):
        os.makedirs(working_directory)
    # Copy gjrand binary files to work around relative paths.
    if testsuite == "gjrand":
        shutil.copy(gjrand_exe, os.path.join(working_directory, "pmcp"))
        shutil.copytree(gjrand_bin, os.path.join(working_directory, "bin"))
    # Run the job.
    run_command(
        gen_args.get_args(str(seed_lo64), str(seed_hi64)), working_directory, dry_run
    )


def run_all_seeds(
    generator,
    testsuite,
    output,
    num_seeds,
    output_shift,
    output_directory,
    dry_run=False,
    mppoolsize=DEFAULT_MP_POOL_SIZE,
):
    # Setup the output directory.
    if os.path.exists(output_directory) and not dry_run:
        print("Removing old results directory: " + output_directory)
        shutil.rmtree(output_directory)
    if not os.path.exists(output_directory) and not dry_run:
        os.makedirs(output_directory)
    # Setup the command to run.
    if (
        testsuite.endswith("crush")
        or testsuite == "alphabit"
        or testsuite == "matrixrank"
        or testsuite == "linearcomp"
    ):
        gen_args = TestU01Arguments(generator, testsuite, output, output_shift)
    if testsuite == "practrand" or testsuite == "gjrand":
        if testsuite == "practrand":
            # -a to display all test results (no p-val threshold)
            # -tlmax 1GB for a short run
            tester_args = "RNG_test stdin64 -a"
        if testsuite == "gjrand":
            # Note that the gjrand pmcp binary has relative references to other
            # executables in a 'bin' folder. To work around this, the pmcp test
            # program and bin folder are copied and run from the working
            # directory.
            # tester_args = './pmcp --standard'
            tester_args = "./pmcp --ten-tera"
        gen_args = CmdLineArguments(generator, output, tester_args)
    # Run jobs in parallel
    with Pool(mppoolsize) as p:
        p.starmap(
            run_test,
            zip(
                list(range(num_seeds)),
                repeat(testsuite),
                repeat(num_seeds),
                repeat(gen_args),
                repeat(output_directory),
                repeat(dry_run),
            ),
        )


def main():
    parser = argparse.ArgumentParser(description="Run a PRNG test set on the queue")
    parser.add_argument("outputdir", help="Specify a directory to write results")
    parser.add_argument("generator", help="Specify a path to the generator binary")
    parser.add_argument(
        "testsuite",
        choices=[
            "smallcrush",
            "crush",
            "bigcrush",
            "alphabit",
            "practrand",
            "gjrand",
            "matrixrank",
            "linearcomp",
        ],
        help="Specify a test suite to run",
    )
    parser.add_argument("output", help="Specify a generator output")
    parser.add_argument(
        "--numseeds",
        type=int,
        default=DEFAULT_NUM_SEEDS,
        help="The number of seeds to test",
    )
    parser.add_argument(
        "--outputshift",
        type=int,
        default=0,
        help="The number of bits to right-shift the output by (only for matrixrank and binarycomp)",
    )
    parser.add_argument("--dryrun", action="store_true", help="Don't run anything")
    parser.add_argument(
        "--mppoolsize",
        type=int,
        default=DEFAULT_MP_POOL_SIZE,
        help="The size of the multiprocessing pool",
    )
    args = parser.parse_args()
    # Check the generator exists
    if not os.path.exists(args.generator) and not os.path.exists(
        os.path.join("bin", args.generator)
    ):
        print(f"Generator {args.generator} could not be found")
        return 1
    # Run the tests.
    run_all_seeds(
        args.generator,
        args.testsuite,
        args.output,
        args.numseeds,
        args.outputshift,
        args.outputdir,
        args.dryrun,
        args.mppoolsize,
    )
    return 0


if __name__ == "__main__":
    gen_args = None
    sys.exit(main())
