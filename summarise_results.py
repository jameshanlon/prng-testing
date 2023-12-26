import argparse
import logging
import os
import re


class Result:
    def __init__(self, filepath, seed):
        self.filepath = filepath
        self.seed = seed
        self.failures = []
        self.successes = []
        self.num_tests_run = 0
        self.length = ""

    def set_num_tests_run(self, num):
        self.num_tests_run = num

    def set_length(self, length):
        self.length = length

    def add_failure(self, test_name, p_value):
        self.failures.append((test_name, p_value))

    def add_success(self, test_name, p_value):
        self.successes.append((test_name, p_value))


def parse_testu01_report(seed, filepath):
    result = Result(filepath, seed)
    with open(filepath, "r") as fp:
        logging.debug(filepath)
        contents = fp.read()
    if "All tests were passed" in contents:
        return result
    m = re.search(
        r"Summary results.*\n(.*\n)+\s*-+\s*\n((.*\n)+)\s*-+\s*\n",
        contents,
        re.MULTILINE,
    )
    if not m:
        raise Exception("No results summary")
    for failure in m.group(2).split("\n"):
        if failure.strip():
            tokens = failure.split("  ")
            result.add_failure(tokens[1].strip(), tokens[2].strip())
    return result


def parse_testu01_single_report(seed, filepath):
    result = Result(filepath, seed)
    if not filepath:
        print(f"Warning: seed {seed} output file does not exist")
        return None
    with open(filepath, "r") as fp:
        logging.debug(filepath)
        contents = fp.read()
    m = re.search(r"p-value of test\s+:\s+(\S+)", contents)
    if not m:
        print(f"Warning: no p-value in {filepath} for seed {seed}")
        return None
    test_p_value = float(m.group(1))
    if test_p_value > 0.999 or test_p_value < 0.001:
        result.add_failure("-", test_p_value)
    else:
        result.add_success("-", test_p_value)
    return result


def parse_practrand_report(seed, filepath):
    # Default reports with only anomolous results listed.
    result = Result(filepath, seed)
    with open(filepath, "r") as fp:
        logging.debug(filepath)
        contents = fp.read()
    last_result_text = contents.strip().split("\n\n")[-1]
    if "no anomalies in" in last_result_text:
        return result
    m = re.search(
        r"Test Name.*\n((.*\n)+)\s+\.\.\.and (\d+) test result\(s\) without anomalies",
        last_result_text,
        re.MULTILINE,
    )
    if not m:
        raise Exception("No results summary")
    result.set_num_tests_run(int(m.group(3)))
    for failure in m.group(1).strip().split("\n"):
        tokens = failure.strip().split("  ")
        result.add_failure(tokens[0], "")
    return result


def parse_practrand_report_full(seed, filepath):
    # Reports with all tests listed (run with -a)
    result = Result(filepath, seed)
    with open(filepath, "r") as fp:
        logging.debug(filepath)
        contents = fp.read()
    last_result_text = contents.strip().split("\n\n")[-1]
    all_result_lines = last_result_text.split("\n")
    # length = all_result_lines[1].split(',')[0]
    # result.set_length(length)
    result_lines = all_result_lines[3:]
    result.set_num_tests_run(len(result_lines))
    for line in result_lines:
        m = re.search(r"p[\s~]=\s*(\S+)", line)
        test_p_value = eval(
            m.group(1)
        )  # Use eval since these numbers can include operators such as '1-7.8e-3'
        test_name = line.strip().split("  ")[0]
        if test_p_value > 0.999 or test_p_value < 0.001:
            result.add_failure(test_name, test_p_value)
    return result


def parse_gjrand_report(seed, filepath):
    result = Result(filepath, seed)
    # if not os.path.exists(filepath):
    #    return result
    with open(filepath, "r") as fp:
        logging.debug(filepath)
        contents = fp.read()
    result_texts = [x.strip() for x in contents.strip().split("============")]
    for result_text in result_texts:
        if "*****" in result_text or "========" in result_text:
            continue
        result_text_lines = result_text.split("\n")
        test_name = result_text_lines[0]
        test_p_value = float(result_text_lines[-1].split()[2])
        # print('{} {}'.format(test_name, test_result))
        if test_p_value > 0.999 or test_p_value < 0.001:
            result.add_failure(test_name, test_p_value)
    return result


def get_first_sge_file(directory):
    """
    Find the first SGE stdout file.
    """
    for filename in os.listdir(directory):
        if re.search(r"\.o\d+", filename):
            return os.path.join(directory, filename)
    return None


def main():
    # yapf: disable
    parser = argparse.ArgumentParser(description='Summarise test results')
    parser.add_argument('suite',
                        choices=['testu01', 'practrand', 'gjrand', 'matrixrank', 'linearcomp'],
                        help='The test suite run')
    parser.add_argument('directory',
                        nargs='+',
                        help='The directory containing the results')
    parser.add_argument('--verbose',
                        action='store_true',
                        help='display information on each seed')
    # yapf: enable
    args = parser.parse_args()
    logging.basicConfig(level=logging.INFO)

    for directory in args.directory:
        results = []
        if not os.path.exists(directory):
            print(f"Directory {directory} does not exist")
            continue
        for seed_dir in os.listdir(directory):
            if not seed_dir.startswith("seed_"):
                continue
            # if args.verbose:
            #    print(seed_dir)
            if args.suite == "testu01" or args.suite == "practrand":
                filepath = get_first_sge_file(os.path.join(directory, seed_dir))
                if args.suite == "testu01":
                    results.append(parse_testu01_report(seed_dir[5:], filepath))
                if args.suite == "practrand":
                    results.append(parse_practrand_report_full(seed_dir[5:], filepath))
            if args.suite == "gjrand":
                # Read the p?report.txt file.
                filepath1 = os.path.join(directory, seed_dir, "report.txt")
                filepath2 = os.path.join(directory, seed_dir, "preport.txt")
                if os.path.exists(filepath1):
                    results.append(parse_gjrand_report(seed_dir[5:], filepath1))
                if os.path.exists(filepath2):
                    results.append(parse_gjrand_report(seed_dir[5:], filepath2))
            if args.suite == "matrixrank" or args.suite == "linearcomp":
                filepath = get_first_sge_file(os.path.join(directory, seed_dir))
                result = parse_testu01_single_report(seed_dir[5:], filepath)
                if result:
                    results.append(result)

        print(f"==== {directory} ====")

        # Details of each seed
        if args.verbose:
            for result in results:
                # failing_tests = ', '.join([x[0] for x in result.failures])
                pvals = ", ".join(str(pval) for _, pval in result.successes)
                print("{:<34} {}".format(hex(int(result.seed)), pvals))
                # print('{:<160} {:<36} {:<4} {:<20} {}'.format(result.filepath,
                #                                              hex(int(result.seed)),
                #                                              len(result.failures),
                #                                              result.length,
                #                                              failing_tests)) # yapf: disable
        # Totals
        total_test_failures = sum([len(x.failures) for x in results])
        total_seed_failures = sum([1 if len(x.failures) else 0 for x in results])
        print("{:<8} results".format(len(results)))
        print("{:<8} total seed failure(s)".format(total_seed_failures))
        print("{:<8} total test failure(s)".format(total_test_failures))
        if args.suite == "practrand" or args.suite == "gjrand":
            total_tests_run = sum([x.num_tests_run for x in results])
            print("{:<8} total tests run".format(total_tests_run))


if __name__ == "__main__":
    main()
