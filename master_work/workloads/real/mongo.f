set $dir=/nova
set $nfiles=1000
set $dirwidth=0
set $filesize=8m
set $iosize=1m
set $meanappendsize=8m
set $ops=1

debug 0

define fileset name=postset,path=$dir,filesize=$filesize,entries=$nfiles,dirwidth=$dirwidth,prealloc=80
define fileset name=postsetdel,path=$dir,filesize=0,entries=$nfiles,dirwidth=$dirwidth,prealloc

define process name=filereader,instances=1
{
  thread name=filereaderthread,memsize=10m,instances=1
  {
    flowop openfile name=openfile1,filesetname=postset,fd=1
    flowop appendfilerand name=appendfilerand1,iosize=$meanappendsize,fd=1
    flowop closefile name=closefile1,fd=1
    flowop openfile name=openfile2,filesetname=postset,fd=1
    flowop readwholefile name=readfile1,fd=1,iosize=$iosize
    flowop closefile name=closefile2,fd=1
    flowop deletefile name=deletefile1,filesetname=postsetdel
    flowop finishoncount name=finish,value=$ops,target=deletefile1
  }
}

create files

run
