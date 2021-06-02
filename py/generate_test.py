#!/usr/bin/env python3
import argparse
import numpy as np
import time
import math


def generate_test(n_teachers, n_students, n_slots, load_factor,
                  teacher_avail_factor, student_avail_factor, distribution_factor):
    np.random.seed(int(time.time()))

    if teacher_avail_factor == 0.0:
        teacher_availability = np.zeros((n_teachers, n_slots)).astype(np.int64)
        for i in range(n_teachers):
            factor = np.random.uniform(low=0.1)
            teacher_availability[i] = np.random.binomial(1, factor, n_slots).astype(np.int64)
    else:
        teacher_availability = np.random.binomial(1, teacher_avail_factor, (n_teachers, n_slots)).astype(np.int64)

    if student_avail_factor == 0.0:
        student_availability = np.zeros((n_students, n_slots)).astype(np.int64)
        for i in range(n_students):
            factor = np.random.uniform(low=0.1)
            student_availability[i] = np.random.binomial(1, factor, n_slots).astype(np.int64)
    else:
        student_availability = np.random.binomial(1, student_avail_factor, (n_students, n_slots)).astype(np.int64)

    max_assignments_for_teachers = np.sum(teacher_availability, axis=1)
    # min_assignments_for_teachers = np.ceil(load_factor * max_assignments_for_teachers)
    max_assignments_for_students = np.sum(student_availability, axis=1)

    requirements = np.zeros((n_teachers, n_students)).astype(np.int64)

    def generate_row(reqs_num):
        _distribution_factor = min(distribution_factor, round(reqs_num / n_students, 2))
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
        requirements[i] = generate_row(max_assignments_for_teachers[i])
        # requirements[i] = generate_row(np.random.randint(low=min_assignments_for_teachers[i],
        #                                                  high=max_assignments_for_teachers[i] + 1))
        sum_reqs += requirements[i]

    for j in range(n_students):
        while sum_reqs[j] > max_assignments_for_students[j]:
            i = np.random.randint(n_teachers)
            while requirements[i][j] == 0:
                i = (i + 1) % n_teachers
            requirements[i][j] -= 1
            sum_reqs[j] -= 1

    # shuffle teachers
    perm = np.random.permutation(n_teachers)
    requirements = requirements[perm]
    teacher_availability = teacher_availability[perm]

    # shuffle students
    perm = np.random.permutation(n_students)
    requirements = requirements.T[perm].T
    student_availability = student_availability[perm]

    return requirements, teacher_availability, student_availability


def get_options():
    argparser = argparse.ArgumentParser()
    argparser.add_argument("n_teachers", type=int)
    argparser.add_argument("n_students", type=int)
    argparser.add_argument("n_slots", type=int)
    argparser.add_argument("-t", "--teacher-avail-factor", type=float, default=0.0,
                           help="average part of week when teachers are available")
    argparser.add_argument("-s", "--student-avail-factor", type=float, default=0.0,
                           help="average part of week when students are available"),
    argparser.add_argument("-l", "--load-factor", type=float, default=0.8,
                           help="minimum load factor for a teacher"),
    argparser.add_argument("-d", "--distribution-factor", type=float, default=0.1,
                           help="average fraction of students assigned to a teacher")
    argparser.add_argument("-n", "--n-tests", type=int, default=1, help="number of tests")
    argparser.add_argument("-p", "--path", type=str, help="output path", default="")
    args = argparser.parse_args()

    return args


def check_correctness(opt):
    if min(opt.n_teachers, opt.n_students, opt.n_slots, opt.n_tests) <= 0:
        raise ValueError("n_teachers, n_students, n_slots, n_tests should be positive integers")
    # if min(opt.load_factor, opt.availability_factor) <= 0 or max(opt.load_factor, opt.availability_factor) > 1:
    #     raise ValueError("load_factor, availability_factor should be in (0; 1]")


def count_mean_avail_factor(availability):
    return round(np.mean(np.sum(availability, axis=1)) / availability.shape[1], 2)


def format_matrix(matrix):
    return '\n'.join(' '.join(str(elem) for elem in row) for row in matrix) + '\n'


if __name__ == "__main__":
    args = get_options()
    # check_correctness(args)

    # for teacher_avail_factor in np.arange(0.0, 0.0, 0.1):
    #     for student_avail_factor in np.arange(0.0, 0.0, 0.1):
    #
    #         args = get_options()
    #
    #         args.teacher_avail_factor = round(teacher_avail_factor, 1)
    #         args.student_avail_factor = round(student_avail_factor, 1)
    #
    #         requirements, teacher_availability, student_availability = \
    #             generate_test(args.n_teachers, args.n_students, args.n_slots, args.load_factor,
    #                           args.teacher_avail_factor,
    #                           args.student_avail_factor, args.distribution_factor)
    #
    #         if args.teacher_avail_factor == 0.0:
    #             taf_str = "nh" + str(count_mean_avail_factor(teacher_availability))
    #         else:
    #             taf_str = str(args.teacher_avail_factor)
    #
    #         if args.student_avail_factor == 0.0:
    #             saf_str = "nh" + str(count_mean_avail_factor(student_availability))
    #         else:
    #             saf_str = str(args.student_avail_factor)
    #
    #         if args.path == "":
    #             args.path = "_".join(["../tests/test", str(args.n_teachers), str(args.n_students), str(args.n_slots),
    #                                   taf_str, saf_str, str(args.distribution_factor)])
    #         print("out filename:", args.path)
    #
    #         total_reqs = np.sum(requirements)
    #         max_possible_reqs = min(np.sum(teacher_availability), np.sum(student_availability))
    #
    #         print(args.teacher_avail_factor, args.student_avail_factor, ":",
    #               total_reqs, "/", max_possible_reqs, "(" + str(total_reqs / max_possible_reqs) + ")")
    #
    #         with open(args.path, "w") as file:
    #             print(args.n_teachers, args.n_students, args.n_slots, sep=' ', file=file)
    #             print(args.teacher_avail_factor, args.student_avail_factor,
    #                   args.distribution_factor, sep=' ', file=file)
    #             file.write("requirements\n" + format_matrix(requirements))
    #             file.write("teacher_availability\n" + format_matrix(teacher_availability))
    #             file.write("student_availability\n" + format_matrix(student_availability))
    #             file.write("\ntotal requirements:" + str(total_reqs) + "/" + str(max_possible_reqs) + "(" + str(
    #                 total_reqs / max_possible_reqs) + ")")

    while True:
        requirements, teacher_availability, student_availability = \
            generate_test(args.n_teachers, args.n_students, args.n_slots, args.load_factor, args.teacher_avail_factor,
                          args.student_avail_factor, args.distribution_factor)
        total_reqs = np.sum(requirements)
        max_possible_reqs = min(np.sum(teacher_availability), np.sum(student_availability))
        break
        if total_reqs / max_possible_reqs >= 0.74:
            break

    max_assignments_for_teachers = np.sum(teacher_availability, axis=1)
    max_assignments_for_students = np.sum(student_availability, axis=1)

    if args.teacher_avail_factor == 0.0:
        args.teacher_avail_factor = count_mean_avail_factor(teacher_availability)

    if args.student_avail_factor == 0.0:
        args.student_avail_factor = count_mean_avail_factor(student_availability)

    if args.path == "":
        args.path = "_".join(["../tests/test", str(args.n_teachers), str(args.n_students), str(args.n_slots),
                              str(args.teacher_avail_factor), str(args.student_avail_factor),
                              str(args.distribution_factor)])
    print("out filename:", args.path)

    # print("teachers:")
    # for index, value in enumerate(np.sum(requirements, axis=1)):
    #     print(str(value) + "/" + str(max_assignments_for_teachers[index]), end=' ')
    # print("\nstudents:")
    # for index, value in enumerate(np.sum(requirements, axis=0)):
    #     print(str(value) + "/" + str(max_assignments_for_students[index]), end=' ')
    # total_reqs = np.sum(requirements)
    # max_possible_reqs = min(np.sum(teacher_availability), np.sum(student_availability))
    print("\ntotal:", total_reqs, "/", max_possible_reqs, "(" + str(total_reqs / max_possible_reqs) + ")")
    print("taf:", args.teacher_avail_factor)
    print("saf:", args.student_avail_factor)

    with open(args.path, "w") as file:
        print(args.n_teachers, args.n_students, args.n_slots, sep=' ', file=file)
        print(args.teacher_avail_factor, args.student_avail_factor,
              args.distribution_factor, sep=' ', file=file)
        file.write("requirements\n" + format_matrix(requirements))
        file.write("teacher_availability\n" + format_matrix(teacher_availability))
        file.write("student_availability\n" + format_matrix(student_availability))
        file.write("\ntotal requirements:" + str(total_reqs) + "/" + str(max_possible_reqs) + "(" + str(total_reqs / max_possible_reqs) + ")")
        # file.write("teacher_availability_factor\n" + str(teacher_avail_factor) + "\n")
        # file.write("student_availability_factor\n" + str(student_avail_factor) + "\n")
