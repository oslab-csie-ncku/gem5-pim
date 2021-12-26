set $dir=/nova
set $filesize=8m # 1m ~ 1g
set $iosize=1m
set $bytes=8 # 1 ~ 1024 (m) # should be the same with $filesize

debug 0

define file name=largefile,path=$dir,size=$filesize,prealloc

define process name=filereader,instances=1
{
  thread name=filereaderthread,memsize=10m,instances=1
  {
    flowop read name=seqread-file,filename=largefile,iosize=$iosize,iters=$bytes
    flowop finishoncount name=finish,value=1,target=seqread-file
  }
}

create files

run
