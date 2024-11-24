import os
import sys

scripts=os.listdir()
cnt=0
total=8

batches=[]
for i in range(0,int((len(scripts)-1)/total)):
    batches.append([])
    



for script in scripts:
    if (not '.py' in script):
        batch=int((cnt+1)/total)
        cnt=cnt+1
        batches[batch-1].append('bash '+script)

script_pattern='taskset --cpu-list #cpu-#cpu bash run_sh'

cpu=21
bulks=[]
for i in range(0,len(batches)):
    bulks.append("run_bulk_" + str(i)+'.sh')
    sys.stdout = open("run_bulk_" + str(i)+'.sh', "a")
    for script in batches[i]:
        # cpu=cpu+1
        print(script, end=' && ')
    print('echo 1')


st=''
sys.stdout = open("run_all.sh", "a")
for bulk in bulks:
    run_pattern=script_pattern
    run_pattern=run_pattern.replace('#cpu',str(cpu)).replace('run_sh',bulk)
    cpu=cpu+1
    print(run_pattern, end=' & ')
print('echo 1')



        
