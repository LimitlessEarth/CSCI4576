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

world_sizes = {"900" : "1000", "2700" : "500", "5400" : "300", "9000" : "200", "12600" : "100"}
np = [1, 9, 25]
distribs = {"row" : 1, "grid" : 2}
sync_types = {"async" : " -a", "sync" : ""}

# Read in the file
filedata = None
with open('batch_files/RossAdam_MT3.sh', 'r') as file :
  filedata = file.read()
  
new = None

for dist in distribs:
    for sync in sync_types:
        for size in world_sizes:
            for n in np:
                if n == 25:
                    tasks = 5
                    nodes = 5
                else:
                    tasks = n
                    nodes = 1
                    
                # find and replace in batch file
                # Replace the target string
                new = filedata.replace('DISTTYPE', str(dist))
                new = new.replace('DISTNUM', str(distribs[dist]))
                new = new.replace('SYNCTYPE', sync)
                new = new.replace('NPNUM', str(n))
                new = new.replace('WORLDSIZE', world_sizes[size])
                new = new.replace('ITERNUM', size)
                new = new.replace('NODEVAR', str(nodes))
                new = new.replace('TASKVAR', str(tasks))
                new = new.replace('SYNCFLAG', sync_types[sync])
                
                print new
            
                # Write the file out again
                # change with dist, np, size, sync
                with open('batch_files/MT3/RossAdam_MT3_' + dist + '_' + sync + '_' + str(size) + '_' + str(n) + '.sh', 'w') as file:
                  file.write(new)
                #cleanShell('sbatch batch_files/MT3/RossAdam_MT3_' + dist + '_' + sync + '_' + str(size) + '_' + str(n) + '.sh')            