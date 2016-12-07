from pipes import quote as shquote
import shlex, os
from subprocess import Popen, PIPE, call

# Get stdout back from command in a string - TODO: error handling...
def cleanShell(command, sudo=False, in_bg=False):
    """
    Run a shell command in bg or wait for return code
    """
    args = shlex.split(shquote(command))
    if sudo:
        args = ['sudo', 'sh', '-c'] + args
    else:
        args = ['sh', '-c'] + args
    proc = Popen(args, stdout=PIPE, stderr=PIPE)
    if in_bg:
        return proc
    else:
        out, err = proc.communicate()
        lines = [i for i in out.split(os.linesep) if i != 0]
        errors = [e for e in err.split(os.linesep) if e != 0]
        return {"stdout" : lines, "stderr" : errors, "rc" : proc.returncode}

print cleanShell("echo Hello")

# Serial
#   Time per problem size
# World sizes: 1
# NP: 1

# Openmp
#   Time per problem size <static/dynamic> <block size> <thread count>
# World sizes 2
# NP: 1

# Openmpi
#   Time per problem size <np> 
# World sizes 3
# NP: 2, 4, 8, 12, 24, 36, 48, 60

# Hybrid
#   Time per problem size <np> <thread count> <mix>
# World sizes 3
# NP-THEAD SPLIT:
#   4 total     2 np 2 mp
#   8 total     4 np 2 mp, 2 np 4 mp
#   12 total    6 np 2 mp, 2 np 6 mp
#   24 total    12 np 2 mp, 8 np 3 mp, 3 np 8 mp, 2 np 12 mp

#   36 total    18 np 2 mp, 12 np 3 mp,  3 np 12 mp, 2 np 18 mp
#   48 total    24 np 2 mp, 12 np 4 mp, 8 np 6 mp, 6 np 8 mp, 4 np 12 mp, 2 np 24 mp
#   60 total    12 np 5 mp, 10 np 6 mp, 6 np 10 mp, 5 np 12 mp


# Cuda
#   later
# World sizes 4

world_sizes = {"480" : 1, 
               "960" : 1, 
               "8160" : 1, 
               "16320" : 1, 
               "32160" : 1, 
               "64320" : 1, 
               "128160" : 2, 
               "256320" : 3, 
               "512160" : 4, 
               "1024320" : 5}
num_iter = [ 100, 50, 25, 20 ]

serial = [1]
mp_threads = {"2" : 1, "4" : 1, "8" : 1, "12" : 1, "24" : 2}
mp_block_sizes = [2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096]

mpi_tasks = {"2" : [1, 1, 2], 
             "4" : [1, 1, 4], 
             "8" : [1, 1, 8], 
             "12" : [1, 1, 12], 
             "24" : [2, 1, 24], 
             "32" : [2, 2, 16], 
             "48" : [3, 2, 24], 
             "60" : [3, 3, 20]}
             
hybrid_dist = {"4" : [[1, 1, 2, 2]], 
               "8" : [[1, 1, 2, 4]], 
               "12" : [[1, 1, 2, 6]], 
               "24" : [[2, 1, 2, 12]], 
               "32" : [[2, 2, 1, 16]], 
               "48" : [[3, 2, 1, 24], [2, 2, 2, 12], [2, 2, 4, 6]], 
               "60" : [[3, 3, 1, 20]], 
               "120" : [[4, 5, 1, 24]]}

type_map = {"nbody_serial" : [serial], "nbody_openmp" : [mp_threads], "nbody_openmpi" : [mpi_tasks], "nbody_openmp_mpi" : [hybrid_dist]}



# Read in the file
filedata = None
with open('../batch/RossAdam_PROJ_comet.sh', 'r') as file :
  filedata = file.read()
  
new = None

size_time = {"480" : "00:5", 
               "960" : "00:10", 
               "8160" : "00:15", 
               "16320" : "00:20", 
               "32160" : "00:25", 
               "64320" : "00:30", 
               "128160" : "00:35", 
               "256320" : "00:50", 
               "512160" : "01:00", 
               "1024320" : "02:00"}
               
size_iter = {"480" : "100", 
               "960" : "60", 
               "8160" : "40", 
               "16320" : "35", 
               "32160" : "30", 
               "64320" : "25", 
               "128160" : "20", 
               "256320" : "15", 
               "512160" : "10", 
               "1024230" : "10"}
               
def replace_and_write(filename, np, pt, size):
    
    if size == "64320" or (filename == "nbody_openmp_mpi" and np == "48"):
        pass
    else:
        return
        
    nodes = 1
    tasks = 1
    thread_statement = ""
    np_per_node = ""
    num_part = size
    extra = ""
    
    thread_string = "export OMP_NUM_THREADS="
    np_string = "--npernode "
    
    if filename == "nbody_serial":
        # pt: 1
        pass
    elif filename == "nbody_openmp":
        # pt: <threads>
        tasks = pt
        thread_statement = thread_string + str(tasks)
        np_per_node = np_string + "1"
    elif filename == "nbody_openmpi":
        # pt: [<size_limit>, <nodes>, <tasks>]
        nodes = pt[1]
        tasks = pt[2]
    elif filename == "nbody_openmp_mpi":
        # pt: [<size_limit>, <nodes>, <tasks>, <threads>]
        if np == "48":
            extra = "_" + str(pt[2]) + "_" + str(pt[3])
        nodes = pt[1]
        tasks = pt[2] * pt[3]
        np_per_node = np_string + str(pt[2])        
        thread_statement = thread_string + str(pt[3])
    else: #block size openmp
        # pt: <threads>
        np_per_node = np_string + "1"
        tasks = pt
        thread_statement = thread_string + str(tasks)
        
    # find and replace in batch file
    # Replace the target string export OMP_NUM_THREADS= --npernode 
    new = filedata.replace('RUNFILE', filename)
    new = new.replace('NPNUM', str(np))
    new = new.replace('NODEVAR', str(nodes))
    new = new.replace('TASKVAR', str(tasks))
    
    new = new.replace('THREAD_STATEMENT', str(thread_statement))
    new = new.replace('NP_PER_NODE', str(np_per_node))
    new = new.replace('NUMITER', str(size_iter[str(size)]))
    new = new.replace('NUMPART', str(num_part))
    
    new = new.replace('TIME', str(size_time[str(size)]))
    new = new.replace('EXTRA', str(extra))
    
    
    print new
    print "###########################################################################################"

    # Write the file out again
    # change with np, size
    with open('../batch/Proj/RossAdam_Proj_' + filename + '_' + str(np) + '_' + str(nodes) + '_' + str(tasks) + '_' + str(size) + '.sh', 'w') as file:
      file.write(new)
    #print cleanShell('sbatch ../batch/Proj/RossAdam_Proj_' + filename + '_' + str(np) + '_' + str(nodes) + '_' + str(tasks) + '_' + str(size) + '.sh')

for typa in type_map: # serial, mp, mpi, hybrid
    for thing in type_map[typa]: # distribution info
        for option in thing:
            if not isinstance(thing, dict):
                for size in world_sizes: # serial
                    if world_sizes[size] <= 1:
                        
                        #print typa, " ", option, " ", size, " ", int(size) % int(option)
                        replace_and_write(typa, option, option, size)
                    
            else:
                if isinstance(thing[option], int): #openmp
                    for size in world_sizes:
                        if world_sizes[size] <= thing[option]:
                        
                            #print typa, " ", option, " ", size, " ", int(size) % int(option)
                            replace_and_write(typa, option, option, size)
                elif isinstance(thing[option][0], list): # openmp_mpi
                    for sub_option in thing[option]:
                        for size in world_sizes:
                            if world_sizes[size] <= sub_option[0]:
                
                                #print typa, " ", option, " ", sub_option, " ", size, " ", int(size) % int(option)
                                replace_and_write(typa, option, sub_option, size)
                            
                else:
                    for size in world_sizes:
                        if world_sizes[size] <= thing[option][0]:  # openmpi
                        
                            #print typa, " ", option, " ", thing[option], " ", size, " ", int(size) % int(option)
                            replace_and_write(typa, option, thing[option], size)
                            
for block_size in mp_block_sizes:
    #print "nbody_openmp" + str(block_size) ," ", block_size, " 12 [1, 12] 16320"
    #cleanShell('cd .. && ./make-varient.sh ' + str(block_size) + ' && cd scripts')
    replace_and_write("nbody_openmp" + str(block_size), "12", "12", 16320)




