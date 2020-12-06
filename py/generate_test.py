import argparse
import numpy as np


def generate_test(n_teachers, n_students, week_length, day_length,
                  load_factor, availability_factor, convenience_factor,
                  out_file):
    teacher_availability = np.random.binomial(1, availability_factor, (n_teachers, week_length)).astype(np.int64)
    # we can't assign more timeslots to a teacher than they are free on
    max_assignments_for_teacher = np.sum(teacher_availability, axis=1)

    requirements = np.zeros((n_teachers, n_students)).astype(np.int64)
    for i in range(n_teachers):
        requirements[i] = np.random.randint(low=0, high=(max_assignments_for_teacher[i] // n_students * 3 + 1),
                                            size=(n_students,))
        while np.sum(requirements[i]) > max_assignments_for_teacher[i] or \
                np.sum(requirements[i]) < week_length * load_factor:
            requirements[i] = np.random.randint(low=0, high=(max_assignments_for_teacher[i] // n_students * 3 + 1),
                                                size=(n_students,))

    teacher_convenience = np.zeros((n_teachers, week_length)).astype(np.int64)
    for i in range(n_teachers):
        for k in range(week_length):
            teacher_convenience[i][k] = np.random.binomial(1, convenience_factor) if teacher_availability[i][k] else 0

    format_matrix = lambda x: '\n'.join(' '.join(str(elem) for elem in row) for row in x) + '\n'

    with open(out_file, "w") as file:
        print(n_teachers, n_students, week_length, day_length, sep=' ', file=file)
        file.write("requirements\n" + format_matrix(requirements))
        file.write("teacher_availability\n" + format_matrix(teacher_availability))
        file.write("teacher_convenience\n" + format_matrix(teacher_convenience))


def get_options():
    argparser = argparse.ArgumentParser()
    argparser.add_argument("n_teachers", type=int)
    argparser.add_argument("n_students", type=int)
    argparser.add_argument("week_length", type=int)
    argparser.add_argument("day_length", type=int)
    argparser.add_argument("-l", "--load-factor", type=float, default=0.66, help="minimum load factor for a teacher")
    argparser.add_argument("-a", "--availability-factor", type=float, default=0.9,
                           help="average part of week when a teacher is available")
    argparser.add_argument("-c", "--convenience-factor", type=float, default=0.8,
                           help="average part of week when it's convenient for a teacher to work")
    argparser.add_argument("-n", "--n-tests", type=int, default=1, help="number of tests")
    argparser.add_argument("-p", "--prefix", type=str, default="../tests/test", help="common prefix for test names")
    args = argparser.parse_args()
    if args.n_teachers <= 0 or args.n_students <= 0 or args.week_length <= 0 or args.day_length <= 0:
        raise ValueError("all integer arguments should be positive")
    if args.week_length % args.day_length != 0:
        raise ValueError("week length should be a multiple of day length")
    return args


if __name__ == "__main__":
    options = get_options()
    for i in range(options.n_tests):
        generate_test(options.n_teachers, options.n_students, options.week_length, options.day_length,
                      options.load_factor, options.availability_factor, options.convenience_factor,
                      options.prefix + str(i))
