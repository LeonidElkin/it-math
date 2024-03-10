import subprocess
import io
import csv

from pathlib import Path 

executions_cnt = 20

root_path = Path(__file__).resolve().parents[1]

blocky_path =  str(root_path) + "/build/parallel_blocky"
parallel_path = str(root_path) + "/build/parallel"
sequential_path = str(root_path) + "/build/sequential"
results_path = str(root_path) + "/py_results"

blocky_header = ['Iterations', 'Time', 'Block_size', 'Threads', 'Eps', 'Grid_size', 'f', 'g']
parallel_header = ['Iterations', 'Time', 'Threads', 'Eps', 'Grid_size', 'f', 'g']
sequential_header = ['Iterations', 'Time', 'Eps', 'Grid_size', 'f', 'g']

def print_to_string(*args, **kwargs):
    out = io.StringIO()
    print(*args, file=out, **kwargs)
    contents = out.getvalue()
    out.close()
    return contents

def average(arr):
    return sum(arr)/len(arr) 

def average_int(arr):
    return sum(arr)//len(arr) 

def execute(args):
    program = subprocess.run(args, capture_output=True)
    return_code = program.returncode
    stdout = program.stdout
    return [return_code, stdout]

iteration = 0
blocky_iterations_cnt = 64
parallel_iterations_cnt = 32
sequential_iterations_cnt = 8

file = open(results_path + "/blocky.csv", 'w', encoding='UTF8', newline='')
writer = csv.writer(file)
writer.writerow(blocky_header)

for gsz in [100, 200, 500, 1000]:
    for eps in [0.1, 0.01]:
        for threads in [1, 2, 4, 8]:
            for bsz in [32, 64]:
                time = []
                iter = []
                for i in range(executions_cnt):
                    blocky = execute([blocky_path, f'--gsz={gsz}', f'--eps={eps}', f'--threads={threads}', f'--bsz={bsz}'])
                    if blocky[0] != 0:
                        writer.writerow(['err', 'err', str(gsz), str(eps), str(threads), str(bsz)])
                    else:
                        output = blocky[1].decode("utf-8").split()
                        iter.append(output[0])
                        time.append(output[1])
                output[0] = str(average_int(list(map(lambda x: int(x), iter))))
                output[1] = print_to_string("%.6f" % average(list(map(lambda x: float(x), time))), end='')
                writer.writerow(output)
                iteration += 1
                print(f'Passed {iteration}/{blocky_iterations_cnt} iterations')

file.close()

print("Blocky is done!")

iteration = 0

file = open(results_path + "/parallel.csv", 'w', encoding='UTF8', newline='')
writer = csv.writer(file)
writer.writerow(parallel_header)

for gsz in [100, 200, 500, 1000]:
    for eps in [0.1, 0.01]:
        for threads in [1, 2, 4, 8]:
            time = []
            iter = []
            for i in range(executions_cnt):
                blocky = execute([parallel_path, f'--gsz={gsz}', f'--eps={eps}', f'--threads={threads}'])
                if blocky[0] != 0:
                    writer.writerow(['err', 'err', str(gsz), str(eps), str(threads)])
                else:
                    output = blocky[1].decode("utf-8").split()
                    iter.append(output[0])
                    time.append(output[1])
            output[0] = str(average_int(list(map(lambda x: int(x), iter))))
            output[1] = print_to_string("%.6f" % average(list(map(lambda x: float(x), time))), end='')
            writer.writerow(output)
            iteration += 1
            print(f'Passed {iteration}/{parallel_iterations_cnt} iterations') 

file.close()

print("Parallel is done!")

iteration = 0

file = open(results_path + "/sequential.csv", 'w', encoding='UTF8', newline='')
writer = csv.writer(file)
writer.writerow(sequential_header)

for gsz in [100, 200, 500, 1000]:
    for eps in [0.1, 0.01]:
        time = []
        iter = []
        for i in range(executions_cnt):
            blocky = execute([sequential_path, f'--gsz={gsz}', f'--eps={eps}'])
            if blocky[0] != 0:
                writer.writerow(['err', 'err', str(gsz), str(eps)])
            else:
                output = blocky[1].decode("utf-8").split()
                iter.append(output[0])
                time.append(output[1])
        output[0] = str(average_int(list(map(lambda x: int(x), iter))))
        output[1] = print_to_string("%.6f" % average(list(map(lambda x: float(x), time))), end='')
        writer.writerow(output)
        iteration += 1
        print(f'Passed {iteration}/{sequential_iterations_cnt} iterations') 

file.close()

print("Sequential is done!")