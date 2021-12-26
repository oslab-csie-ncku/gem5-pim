set $dir=/nova
set $filesize=2g
set $iosize=1m
set $bytes=8 # 1 ~ 1024 (m)

debug 0

define file name=bigfile1,path=$dir,size=$filesize,prealloc

define process name=filereader,instances=1
{
  thread name=filereaderthread,memsize=10m,instances=1
  {
    flowop read name=read-file,filesetname=bigfile1,random,iosize=$iosize,iters=$bytes
    flowop finishoncount name=finish,value=1,target=read-file
  }
}

create files

run
