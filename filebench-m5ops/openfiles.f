set $dir=d1
set $meandirwidth=50000
set $nfiles=5000
set $count=50

debug 0

define randvar name=$fileidx, type=gamma, min=1, gamma=30000

define fileset name=d2,path=$dir,entries=$nfiles,filesize=0,dirwidth=$meandirwidth,prealloc

define process name=fileopene,instances=1
{
  thread name=fileopener,memsize=1m,instances=1
  {
    flowop openfile name=open1,filesetname=d2,indexed=$fileidx,fd=1
    flowop closefile name=close1,fd=1
    flowop finishoncount name=finish,value=$count,target=close1
  }
}

create files

run
