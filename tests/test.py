import os
import sys
import subprocess

if len(sys.argv) != 3:
    print("Usage: python script.py <dir> <file>")
    sys.exit(1)

dir = sys.argv[1]
file = sys.argv[2]
exec_file = os.path.join(dir, "build", "psForm.out")
real = os.path.join(dir, "tests", "test.txt")
ref = file + ".ans"

# Запуск исполняемого файла и перенаправление ввода/вывода
with open(file, 'r') as infile, open(real, 'w') as outfile:
    subprocess.run([exec_file], stdin=infile, stdout=outfile, stderr=subprocess.STDOUT)

with open(real, 'r') as realf, open(ref, 'r') as reff:
    line1 = realf.readline()  # Читаем первую строку
    real_output = list(map(str, line1.split()))
    line2 = reff.readline()  # Читаем первую строку
    ref_output = list(map(str, line2.split()))

# Удаление временного файла
os.remove(real)

output_real = list()
sign = "+"
for i in real_output:
    if i == '+' or i == '-':
        sign = i
    else:
        res_list = i.split('*')
        mult = 1
        if res_list[0].isdigit() or (res_list[0][:1] == '-' and res_list[0][1:].isdigit()):
            mult = mult * int(res_list[0])
            del res_list[0]
        res_dict = dict()
        for i in res_list:
            if res_dict.get(i):
                res_dict[i] = res_dict[i] + 1
            else:
                res_dict[i] = 1
        if sign == '-':
            mult = mult * (-1)
        res = [mult, res_dict]
        output_real.append(res)

output_ref = list()
sign = "+"
for i in ref_output:
    if i == '+' or i == '-':
        sign = i
    else:
        res_list = i.split('*')
        mult = 1
        if res_list[0].isdigit() or ((res_list[0][:1] == '-') and (res_list[0][1:].isdigit())):
            mult = mult * int(res_list[0])
            del res_list[0]
        res_dict = dict()
        for i in res_list:
            if res_dict.get(i):
                res_dict[i] = res_dict[i] + 1
            else:
                res_dict[i] = 1
        if sign == '-':
            mult = mult * (-1)
        res = [mult, res_dict]
        output_ref.append(res)

res = True
for i in output_ref:
    if i not in output_real:
        print(i, "not in real")
        res = False
        break
for i in output_real:
    if i not in output_ref:
        print(i, "not in ref")
        res = False
        break
# Сравнение содержимого
if res:
    print("Files match.")
    sys.exit(0)
else:
    print("Files do not match.")
    print("real\n", real_output, '\n', output_real)
    print("ref\n", ref_output,  '\n', output_ref)
    sys.exit(1)
