import argparse
import numpy as np


def generate_test(n_teachers, n_students, n_slots, load_factor, availability_factor, out_filename):
    teacher_availability = np.random.binomial(1, availability_factor, (n_teachers, n_slots)).astype(np.int64)
    student_availability = np.random.binomial(1, availability_factor, (n_students, n_slots)).astype(np.int64)

    max_assignments_for_teachers = np.sum(teacher_availability, axis=1)
    min_assignments_for_teachers = load_factor * max_assignments_for_teachers
    max_assignments_for_students = np.sum(student_availability, axis=1)

    requirements = np.zeros((n_teachers, n_students)).astype(np.int64)

    def generate_row(reqs_num):
        row = np.random.random(size=(n_students,))
        row = np.floor(row / row.sum() * reqs_num).astype(np.int64)
        return row

    while True:
        for i in range(n_teachers):
            # requirements[i] = np.random.randint(low=0, high=(max_assignments_for_teachers[i] // n_students * 3 + 1),
            #                                     size=(n_students,))
            # while np.sum(requirements[i]) > max_assignments_for_teachers[i] or \
            #         np.sum(requirements[i]) < n_slots * load_factor:
            #     requirements[i] = np.random.randint(low=0, high=(max_assignments_for_teachers[i] // n_students * 3 + 1),
            #                                         size=(n_students,))
            requirements[i] = generate_row(np.random.randint(low=min_assignments_for_teachers[i],
                                                             high=max_assignments_for_teachers[i]))
        if (np.sum(requirements, axis=0) <= max_assignments_for_students).all():
            break

    format_matrix = lambda x: '\n'.join(' '.join(str(elem) for elem in row) for row in x) + '\n'

    with open(out_filename, "w") as file:
        print(n_teachers, n_students, n_slots, sep=' ', file=file)
        file.write("requirements\n" + format_matrix(requirements))
        file.write("teacher_availability\n" + format_matrix(teacher_availability))
        file.write("student_availability\n" + format_matrix(student_availability))


def get_options():
    argparser = argparse.ArgumentParser()
    argparser.add_argument("n_teachers", type=int)
    argparser.add_argument("n_students", type=int)
    argparser.add_argument("n_slots", type=int)
    argparser.add_argument("-l", "--load-factor", type=float, default=0.5, help="minimum load factor for a teacher")
    argparser.add_argument("-a", "--availability-factor", type=float, default=0.9,
                           help="average part of week when teachers are available")
    argparser.add_argument("-n", "--n-tests", type=int, default=1, help="number of tests")
    argparser.add_argument("-p", "--prefix", type=str, default="../tests/test", help="common prefix for test names")
    return argparser.parse_args()


def check_correctness(opt):
    if min(opt.n_teachers, opt.n_students, opt.n_slots, opt.n_tests) <= 0:
        raise ValueError("n_teachers, n_students, n_slots, n_tests should be positive integers")
    # if min(opt.load_factor, opt.availability_factor) <= 0 or max(opt.load_factor, opt.availability_factor) > 1:
    #     raise ValueError("load_factor, availability_factor should be in (0; 1]")


if __name__ == "__main__":
    opt = get_options()
    check_correctness(opt)
    for i in range(opt.n_tests):
        generate_test(opt.n_teachers, opt.n_students, opt.n_slots, opt.load_factor, opt.availability_factor,
                      opt.prefix + str(i))
