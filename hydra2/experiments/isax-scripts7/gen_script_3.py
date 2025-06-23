infos=[('OBS',1000000,256,300),('PNW',1000000,256,300),('OBST2024',1000000,256,300),('NEIC',1000000,256,300),('Meier2019JGR',1000000,256,300),('ISC_EHB_DepthPhases',1000000,256,300),('Iquique',578853,256,300)]


import sys
final_run=''
content=open("template.sh", "r").read()
for info in infos:
    ds=info[0]
    n=str(info[1])
    d=str(info[2])
    qn=str(info[3])
    log_file_name = ds+".sh"
    final_run=final_run+'bash '+log_file_name+' & '
    sys.stdout = open(log_file_name, "w")
    new_content=content+''
    new_content=new_content.replace('[DATASET_SIZE]',n)
    new_content=new_content.replace('[DIMENSION]',d)
    new_content=new_content.replace('[DATASET]',ds)
    new_content=new_content.replace('[QUERY_SIZE]',qn)
    print(new_content)
final_run=final_run+'echo 1'
sys.stdout = open('run_all.sh', "w")          
print(final_run)           
    
    
