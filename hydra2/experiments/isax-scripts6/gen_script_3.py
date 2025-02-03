infos=[('ethz',36643,256,100),('vcseis',160178,256,100),('txed',519589,256,100),('lendb',1000000,256,100),('stead',1000000,256,100),('geofon',275174,128,100),('instancegm',1000000,128,100),('Music',1000000,100,100),('Yelp',77079,50,100)]


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
    print(new_content)
final_run=final_run+'echo 1'
sys.stdout = open('run_all.sh', "w")          
print(final_run)           
    
    