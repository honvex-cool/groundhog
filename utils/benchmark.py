import matplotlib
import matplotlib.pyplot as plt
import multiprocessing
import os
import pathlib
import sys

INVALID_USAGE = 1
WRONG_ANSWER = 3
RUNTIME_ERROR = 4

INDIFFERENT_NAME = 'sequential'

MODIFIER = 1e-9
UNIT = 's'

def main():
    if len(sys.argv) != 8:
        sys.exit(INVALID_USAGE)
    machine, exe_dir, test_in, test_ok, out, reports, charts = sys.argv[1:]
    out_name = pathlib.Path(out).stem
    chart_dir = os.path.join(charts, machine, out_name)
    ensure_dirs(chart_dir)
    cpu_counts = list(range(2, multiprocessing.cpu_count() + 1))
    test_name = pathlib.Path(test_in).stem
    entries = [entry for entry in os.listdir(exe_dir) if entry.endswith('.x')]
    legend = []
    for exe, color in zip(entries, matplotlib.colors.TABLEAU_COLORS):
        stem = pathlib.Path(exe).stem
        legend.append(stem)
        executable = os.path.join(exe_dir, exe)
        sub_out_path = os.path.join(out, machine, stem)
        sub_report_path = os.path.join(reports, machine, stem)
        ensure_dirs(sub_out_path, sub_report_path)
        scores = []
        for c in cpu_counts:
            common = test_name + '_' + str(c).rjust(2, '0')
            test_out = os.path.join(sub_out_path, common + '.out')
            report_out = os.path.join(sub_report_path, common + '.txt')
            run_cmd = f'{executable} {c} < {test_in} > {test_out} 2> {report_out}'
            if os.waitstatus_to_exitcode(os.system(run_cmd)) != 0:
                sys.exit(RUNTIME_ERROR)
            check_cmd = f'diff {test_out} {test_ok}'
            if os.waitstatus_to_exitcode(os.system(check_cmd)) != 0:
                sys.exit(WRONG_ANSWER)
            summary = as_dictionary(report_out)
            avg = summary['sol_tot'] / summary['n_tests']
            if stem == INDIFFERENT_NAME:
                scores = [avg for _ in cpu_counts]
                break
            scores.append(avg)
        plt.plot(cpu_counts, [v * MODIFIER for v in scores], color=color)
    plt.xlabel('available CPUs')
    plt.ylabel(f'time ({UNIT})')
    plt.suptitle(out_name)
    plt.title(machine)
    plt.legend(legend)
    plt.savefig(os.path.join(chart_dir, test_name + '.png'), bbox_inches='tight')

def ensure_dirs(*paths):
    for path in paths:
        pathlib.Path(path).mkdir(parents=True, exist_ok=True)

def as_dictionary(report_path):
    result = {}
    with open(report_path) as f:
        for line in f:
            if not line:
                continue
            x = line.split(' ')
            result[x[0]] = int(x[1])
    return result

if __name__ == '__main__':
    main()
