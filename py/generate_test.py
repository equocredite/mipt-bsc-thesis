import argparse
import numpy as np
import time


def generate_test(n_teachers, n_students, n_slots, load_factor,
                  teacher_avail_factor, student_avail_factor, distribution_factor, out_filename):
    np.random.seed(int(time.time()))

    teacher_availability = np.random.binomial(1, teacher_avail_factor, (n_teachers, n_slots)).astype(np.int64)
    student_availability = np.random.binomial(1, student_avail_factor, (n_students, n_slots)).astype(np.int64)

    max_assignments_for_teachers = np.sum(teacher_availability, axis=1)
    min_assignments_for_teachers = np.ceil(load_factor * max_assignments_for_teachers)
    max_assignments_for_students = np.sum(student_availability, axis=1)

    print(min_assignments_for_teachers, max_assignments_for_teachers, max_assignments_for_students, sep='\n')

    requirements = np.zeros((n_teachers, n_students)).astype(np.int64)

    def generate_row(reqs_num):
        # indicates whether each student should be considered at all
        row = np.random.binomial(1, distribution_factor, (n_students,)).astype(np.int64)
        n_ones = np.count_nonzero(row)
        # first obtain fractions, then scale them to reqs_num
        reqs = np.random.random(size=(n_ones,))
        reqs = np.floor(reqs / reqs.sum() * reqs_num).astype(np.int64)

        j = 0
        for i in range(n_students):
            if row[i] == 1:
                row[i] = reqs[j]
                j += 1
        return row

    sum_reqs = np.zeros(n_students).astype(np.int64)
    for i in range(n_teachers):
        requirements[i] = generate_row(np.random.randint(low=min_assignments_for_teachers[i],
                                                         high=max_assignments_for_teachers[i] + 1))
        sum_reqs += requirements[i]

    while (sum_reqs > max_assignments_for_students).any():
        print(np.sum(sum_reqs))
        for j in range(n_students):
            if sum_reqs[j] > max_assignments_for_students[j]:
                i = np.random.randint(n_teachers)
                for iter in range(n_teachers):
                    if requirements[i][j] > 0:
                        requirements[i][j] -= 1
                        sum_reqs[j] -= 1
                        break
                    i = (i + 1) % n_teachers

    # shuffle teachers
    perm = np.random.permutation(n_teachers)
    requirements = requirements[perm]
    teacher_availability = teacher_availability[perm]

    # shuffle students
    perm = np.random.permutation(n_students)
    requirements = requirements.T[perm].T
    student_availability = student_availability[perm]

    format_matrix = lambda x: '\n'.join(' '.join(str(elem) for elem in row) for row in x) + '\n'

    with open(out_filename, "w") as file:
        print(n_teachers, n_students, n_slots, sep=' ', file=file)
        file.write("requirements\n" + format_matrix(requirements))
        file.write("teacher_availability\n" + format_matrix(teacher_availability))
        file.write("student_availability\n" + format_matrix(student_availability))
        file.write("teacher_availability_factor\n" + str(teacher_avail_factor) + "\n")
        file.write("student_availability_factor\n" + str(student_avail_factor) + "\n")


def get_options():
    argparser = argparse.ArgumentParser()
    argparser.add_argument("n_teachers", type=int)
    argparser.add_argument("n_students", type=int)
    argparser.add_argument("n_slots", type=int)
    argparser.add_argument("-t", "--teacher-avail-factor", type=float, default=0.9,
                           help="average part of week when teachers are available")
    argparser.add_argument("-s", "--student-avail-factor", type=float, default=0.9,
                           help="average part of week when students are available"),
    argparser.add_argument("-l", "--load-factor", type=float, default=0.8,
                           help="minimum load factor for a teacher"),
    argparser.add_argument("-d", "--distribution-factor", type=float, default=0.3,
                           help="average number of students assigned to a teacher")
    argparser.add_argument("-n", "--n-tests", type=int, default=1, help="number of tests")
    argparser.add_argument("-p", "--path", type=str, help="output path", default="")
    args = argparser.parse_args()
    if args.path == "":
        args.path = "_".join(["../tests/test", str(args.n_teachers), str(args.n_students), str(args.n_slots),
                              str(args.teacher_avail_factor), str(args.student_avail_factor), str(args.load_factor),
                              str(args.distribution_factor)])
    return args


def check_correctness(opt):
    if min(opt.n_teachers, opt.n_students, opt.n_slots, opt.n_tests) <= 0:
        raise ValueError("n_teachers, n_students, n_slots, n_tests should be positive integers")
    # if min(opt.load_factor, opt.availability_factor) <= 0 or max(opt.load_factor, opt.availability_factor) > 1:
    #     raise ValueError("load_factor, availability_factor should be in (0; 1]")


if __name__ == "__main__":
    opt = get_options()
    check_correctness(opt)
    generate_test(opt.n_teachers, opt.n_students, opt.n_slots, opt.load_factor, opt.teacher_avail_factor,
                  opt.student_avail_factor, opt.distribution_factor, opt.path)
