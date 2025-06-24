infos=[('ethz',36643,256,100),('vcseis',160178,256,100),('txed',519589,256,100),('lendb',1000000,256,100),('stead',1000000,256,100),('geofon',275174,128,100),('instancegm',1000000,128,100),('Music',1000000,100,100),('Yelp',77079,50,100),('OBS',1000000,256,300),('PNW',1000000,256,300),('OBST2024',1000000,256,300),('NEIC',1000000,256,300),('Meier2019JGR',1000000,256,300),('ISC_EHB_DepthPhases',1000000,256,300),('Iquique',578853,256,300)]

cpus=[7,8,9,10,11,12,13,14,15,16,21,22,23,24,25,26,27,61,62,63,64,65,66,67,68,69,70]

import sys
final_run=''
content=open("template.sh", "r").read()
cnt=0
for info in infos:
    ds=info[0]
    n=str(info[1])
    d=str(info[2])
    qn=str(info[3])
    log_file_name = ds+".sh"
    final_run=final_run+'taskset --cpu-list [cpu]-[cpu] '.replace('[cpu]',str(cpus[cnt]))+'bash '+log_file_name+' & '
    cnt=cnt+1
    sys.stdout = open(log_file_name, "w")
    new_content=content+''
    new_content=new_content.replace('[DATASET_SIZE]',n)
    new_content=new_content.replace('[DIMENSION]',d)
    new_content=new_content.replace('[K]','100')
    new_content=new_content.replace('[DATASET]',ds)
    new_content=new_content.replace('[QUERY_SIZE]',qn)
    print(new_content)
final_run=final_run+'echo 1'
sys.stdout = open('run_all.sh', "w")          
print(final_run)           
    
    
